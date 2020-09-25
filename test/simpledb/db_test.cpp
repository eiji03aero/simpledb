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
  void SetUp() override {
    const char *pathname {"/app/build/simpledb"};
    const char *const argv[] = {nullptr};
    process.execvp(pathname, argv);
  }

  void TearDown() override {
    process.close();
    process.kill();
  }

  std::vector<std::string> exec_statements(std::vector<std::string> statements) {
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

    return result;
  }

  pipedcp::Process process;
};

TEST_F(DbTest, InputOutputCorrect) {
  std::vector<std::string> result = exec_statements(std::vector<std::string> {
    "insert 1 user1 person1@example.com\n",
    "select\n",
    ".exit\n"
  });

  std::vector<std::string> expected {
    "db > executed",
    "db > 1 user1 person1@example.com",
    "executed",
    "db > "
  };
  EXPECT_EQ(result.size(), expected.size());
  for (int i {0}; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
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

  std::vector<std::string> result = exec_statements(std::vector<std::string> {
    insert_stream.str(),
    "select\n",
    ".exit\n"
  });

  std::stringstream select_stream;
  select_stream << "db > " << 1 << " " << name << " " << email;
  std::vector<std::string> expected {
    "db > executed",
    select_stream.str(),
    "executed",
    "db > "
  };
  EXPECT_EQ(result.size(), expected.size());
  for (int i {0}; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
};

TEST_F(DbTest, EmitsStringTooLongError) {
  std::string name;
  name.insert(0, COLUMN_USERNAME_SIZE + 1, 'n');
  std::string email;
  email.insert(0, COLUMN_EMAIL_SIZE + 1, 'e');
  std::stringstream insert_stream;
  insert_stream << "insert 1 " << name << " " << email << '\n';

  std::vector<std::string> result = exec_statements(std::vector<std::string> {
    insert_stream.str(),
    "select\n",
    ".exit\n"
  });

  std::vector<std::string> expected {
    "db > Error: string is too long",
    "db > executed",
    "db > "
  };
  EXPECT_EQ(result.size(), expected.size());
  for (int i {0}; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
}

TEST_F(DbTest, EmitsNegativeIdError) {
  std::vector<std::string> result = exec_statements(std::vector<std::string> {
    "insert -1 domo hoge@example.com\n",
    "select\n",
    ".exit\n"
  });

  std::vector<std::string> expected {
    "db > Error: id must be positive",
    "db > executed",
    "db > "
  };
  EXPECT_EQ(result.size(), expected.size());
  for (int i {0}; i < result.size(); i++) {
    EXPECT_EQ(result[i], expected[i]);
  }
}

} /* namespace simpledb */
