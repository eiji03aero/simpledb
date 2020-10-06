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
  int num_rows { 1301 };
  std::vector<std::string> statements;
  std::stringstream statement_stream;
  for (int i {1}; i < num_rows + 1; i++) {
    statement_stream << "insert " << i << " user" << i << " person" << i << "@example.com\n";
    statements.push_back(statement_stream.str());
    statement_stream.str("");
  }

  std::vector<std::string> result = exec_statements(statements);

  EXPECT_EQ(result.at(result.size() - 2), "db > Error: table full");
  EXPECT_EQ(result.at(result.size() - 3), "db > executed");
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
    "LEAF_NODE_HEADER_SIZE: 10",
    "LEAF_NODE_CELL_SIZE: 297",
    "LEAF_NODE_SPACE_FOR_CELLS: 4086",
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
    "db > Error: need to implement searching an internal node",
  };

  expect_statements_result(statements, expected);
}

} /* namespace simpledb */
