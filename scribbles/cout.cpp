#include <sstream>

#include "gtest/gtest.h"

TEST(ExampleTest, Sample1) {
  std::stringstream buffer;
  std::streambuf *sbuf = std::cout.rdbuf();

  std::cout.rdbuf(buffer.rdbuf());

  std::cout << "Hello world";

  std::cout.rdbuf(sbuf);

  std::cout << "done" << '\n';
  std::cout << buffer.str() << '\n';

  EXPECT_TRUE(buffer.str() == "Hello world");
};
