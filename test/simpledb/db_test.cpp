#include "simpledb/row.h"
#include "simpledb/table.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>

#include "gtest/gtest.h"

#include "pipedcp/process.h"

namespace simpledb {

class DbTest : public ::testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override {
    remove("mydb.db");
  }

  std::vector<std::string> exec_statements(std::vector<std::string> statements) {
    start_db();

    for (std::string statement : statements) {
      write(process.writefd(), statement.c_str(), strlen(statement.c_str()));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });

    // Gave up dynamically allocate/read from piped fd.
    // Just allocate gigantic buffer
    const size_t buffer_size { 30000 };
    char read_buffer[buffer_size] = {0};
    read(process.readfd(), read_buffer, buffer_size);

    std::stringstream buffer_stream { read_buffer };
    std::vector<std::string> result;
    std::string line;
    while(std::getline(buffer_stream, line)) {
      if (!line.empty()) {
        result.push_back(line);
      }
    }

    terminate_db();
    return result;
  }

  void expect_statements_result(std::vector<std::string> statements, std::vector<std::string> expected) {
    std::vector<std::string> result { exec_statements(statements) };

    EXPECT_EQ(result.size(), expected.size());
    for (int i {0}; i < result.size(); i++) {
      EXPECT_EQ(result[i], expected[i]);
    }
  }

  void start_db() {
    process = pipedcp::Process{};
    const char *pathname {"/app/build/simpledb"};
    const char *const argv[] = {"./simpledb", "mydb.db", nullptr};
    process.execvp(pathname, argv);
  }

  void terminate_db() {
    process.close();
    process.kill();
  }

  pipedcp::Process process;
};

TEST_F(DbTest, InputOutputCorrect) {
  std::vector<std::string> statements {
    "insert 1 user1 person1@example.com\n",
    "select\n",
    ".exit\n"
  };

  std::vector<std::string> expected {
    "db > executed",
    "db > 1 user1 person1@example.com",
    "executed",
    "db > "
  };

  expect_statements_result(statements, expected);
};

TEST_F(DbTest, EmitsTableFullError) {
  int num_rows { 1401 };
  std::vector<std::string> statements;
  std::stringstream statement_stream;
  for (int i {1}; i < num_rows + 1; i++) {
    statement_stream << "insert " << i << " user" << i << " person" << i << "@example.com\n";
    statements.push_back(statement_stream.str());
    statement_stream.str("");
  }

  std::vector<std::string> result = exec_statements(statements);

  EXPECT_EQ(result.at(result.size() - 2), "db > executed");
  EXPECT_EQ(result.at(result.size() - 1), "db > Error: need to implement splitting internal node");
};

TEST_F(DbTest, InsertsRowWithMaximumLength) {
  std::string name;
  name.insert(0, COLUMN_USERNAME_SIZE, 'n');
  std::string email;
  email.insert(0, COLUMN_EMAIL_SIZE, 'e');
  std::stringstream insert_stream;
  insert_stream << "insert 1 " << name << " " << email << '\n';
  std::vector<std::string> statements {
    insert_stream.str(),
    "select\n",
    ".exit\n"
  };

  std::stringstream select_stream;
  select_stream << "db > " << 1 << " " << name << " " << email;
  std::vector<std::string> expected {
    "db > executed",
    select_stream.str(),
    "executed",
    "db > "
  };

  expect_statements_result(statements, expected);
};

TEST_F(DbTest, EmitsStringTooLongError) {
  std::string name;
  name.insert(0, COLUMN_USERNAME_SIZE + 1, 'n');
  std::string email;
  email.insert(0, COLUMN_EMAIL_SIZE + 1, 'e');
  std::stringstream insert_stream;
  insert_stream << "insert 1 " << name << " " << email << '\n';

  std::vector<std::string> statements {
    insert_stream.str(),
    "select\n",
    ".exit\n"
  };

  std::vector<std::string> expected {
    "db > Error: string is too long",
    "db > executed",
    "db > "
  };

  expect_statements_result(statements, expected);
}

TEST_F(DbTest, EmitsNegativeIdError) {
  std::vector<std::string> statements {
    "insert -1 domo hoge@example.com\n",
    "select\n",
    ".exit\n"
  };

  std::vector<std::string> expected {
    "db > Error: id must be positive",
    "db > executed",
    "db > "
  };

  expect_statements_result(statements, expected);
}

TEST_F(DbTest, KeepsDataAfterClosingConnection) {
  std::vector<std::string> statements1 {
    "insert 1 user1 user1@example.com\n",
    ".exit\n",
  };

  std::vector<std::string> expected1 {
    "db > executed",
    "db > ",
  };

  expect_statements_result(statements1, expected1);

  std::vector<std::string> statements2 {
    "select\n",
    ".exit\n",
  };

  std::vector<std::string> expected2 {
    "db > 1 user1 user1@example.com",
    "executed",
    "db > ",
  };

  expect_statements_result(statements2, expected2);
}

TEST_F(DbTest, PrintsConstants) {
  std::vector<std::string> statements {
    ".constants\n",
    ".exit\n",
  };

  std::vector<std::string> expected {
    "db > Constants: ",
    "ROW_SIZE: 293",
    "COMMON_NODE_HEADER_SIZE: 6",
    "LEAF_NODE_HEADER_SIZE: 14",
    "LEAF_NODE_CELL_SIZE: 297",
    "LEAF_NODE_SPACE_FOR_CELLS: 4082",
    "LEAF_NODE_MAX_CELLS: 13",
    "db > ",
  };

  expect_statements_result(statements, expected);
}

TEST_F(DbTest, PrintsStructureOfOneNodeBTree) {
  std::vector<std::string> statements {
    "insert 3 user3 person3@example.com\n",
    "insert 1 user1 person1@example.com\n",
    "insert 2 user2 person2@example.com\n",
    ".btree\n",
    ".exit\n",
  };

  std::vector<std::string> expected {
    "db > executed",
    "db > executed",
    "db > executed",
    "db > Tree: ",
    "- leaf (size 3)",
    "  - 1",
    "  - 2",
    "  - 3",
    "db > ",
  };

  expect_statements_result(statements, expected);
}

TEST_F(DbTest, PrintsErrorDuplicateId) {
  std::vector<std::string> statements {
    "insert 1 user1 person1@example.com\n",
    "insert 1 user1 person1@example.com\n",
    "select\n",
    ".exit\n",
  };

  std::vector<std::string> expected {
    "db > executed",
    "db > Error: duplicate key",
    "db > 1 user1 person1@example.comyade",
    "executed",
    "db > ",
  };

  expect_statements_result(statements, expected);
}

TEST_F(DbTest, PrintsStructureOf3LeafNode) {
  std::vector<std::string> statements;
  for (uint32_t i = 1; i < 15; i++) {
    std::stringstream ss;
    ss << "insert " << i << " user" << i << " person" << i << "@example.com" << '\n';
    statements.push_back(ss.str());
  }
  statements.push_back(".btree\n");
  statements.push_back("insert 15 user15 person15@example.com\n");
  statements.push_back(".exit\n");

  std::vector<std::string> expected {
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > Tree: ",
    "- internal (size 1)",
    "  - leaf (size 7)",
    "    - 1",
    "    - 2",
    "    - 3",
    "    - 4",
    "    - 5",
    "    - 6",
    "    - 7",
    "  - key 7",
    "  - leaf (size 7)",
    "    - 8",
    "    - 9",
    "    - 10",
    "    - 11",
    "    - 12",
    "    - 13",
    "    - 14",
    "db > executed",
    "db > "
  };

  expect_statements_result(statements, expected);
}

TEST_F(DbTest, PrintsAllRowsInMultiLevelTree) {
  std::vector<std::string> statements;
  for (uint32_t i = 1; i < 16; i++) {
    std::stringstream ss;
    ss << "insert " << i << " user" << i << " person" << i << "@example.com" << '\n';
    statements.push_back(ss.str());
  }
  statements.push_back("select\n");
  statements.push_back(".exit\n");

  std::vector<std::string> expected {
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > 1 user1 person1@example.com",
    "2 user2 person2@example.com",
    "3 user3 person3@example.com",
    "4 user4 person4@example.com",
    "5 user5 person5@example.com",
    "6 user6 person6@example.com",
    "7 user7 person7@example.com",
    "8 user8 person8@example.com",
    "9 user9 person9@example.com",
    "10 user10 person10@example.com",
    "11 user11 person11@example.com",
    "12 user12 person12@example.com",
    "13 user13 person13@example.com",
    "14 user14 person14@example.com",
    "15 user15 person15@example.com",
    "executed",
    "db > "
  };

  expect_statements_result(statements, expected);
}

TEST_F(DbTest, PrintsStructureOf4LeafNodeBTree) {
  std::vector<std::string> statements {
    "insert 18 user18 person18@example.com\n",
    "insert 7 user7 person7@example.com\n",
    "insert 10 user10 person10@example.com\n",
    "insert 29 user29 person29@example.com\n",
    "insert 23 user23 person23@example.com\n",
    "insert 4 user4 person4@example.com\n",
    "insert 14 user14 person14@example.com\n",
    "insert 30 user30 person30@example.com\n",
    "insert 15 user15 person15@example.com\n",
    "insert 26 user26 person26@example.com\n",
    "insert 22 user22 person22@example.com\n",
    "insert 19 user19 person19@example.com\n",
    "insert 2 user2 person2@example.com\n",
    "insert 1 user1 person1@example.com\n",
    "insert 21 user21 person21@example.com\n",
    "insert 11 user11 person11@example.com\n",
    "insert 6 user6 person6@example.com\n",
    "insert 20 user20 person20@example.com\n",
    "insert 5 user5 person5@example.com\n",
    "insert 8 user8 person8@example.com\n",
    "insert 9 user9 person9@example.com\n",
    "insert 3 user3 person3@example.com\n",
    "insert 12 user12 person12@example.com\n",
    "insert 27 user27 person27@example.com\n",
    "insert 17 user17 person17@example.com\n",
    "insert 16 user16 person16@example.com\n",
    "insert 13 user13 person13@example.com\n",
    "insert 24 user24 person24@example.com\n",
    "insert 25 user25 person25@example.com\n",
    "insert 28 user28 person28@example.com\n",
    ".btree\n",
    ".exit\n",
  };

  std::vector<std::string> expected {
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > executed",
    "db > Tree: ",
    "- internal (size 3)",
    "  - leaf (size 7)",
    "    - 1",
    "    - 2",
    "    - 3",
    "    - 4",
    "    - 5",
    "    - 6",
    "    - 7",
    "  - key 1",
    "  - leaf (size 8)",
    "    - 8",
    "    - 9",
    "    - 10",
    "    - 11",
    "    - 12",
    "    - 13",
    "    - 14",
    "    - 15",
    "  - key 15",
    "  - leaf (size 7)",
    "    - 16",
    "    - 17",
    "    - 18",
    "    - 19",
    "    - 20",
    "    - 21",
    "    - 22",
    "  - key 22",
    "  - leaf (size 8)",
    "    - 23",
    "    - 24",
    "    - 25",
    "    - 26",
    "    - 27",
    "    - 28",
    "    - 29",
    "    - 30",
    "db > ",
  };

  expect_statements_result(statements, expected);
}

} /* namespace simpledb */
