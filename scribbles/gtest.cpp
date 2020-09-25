#include "gtest/gtest.h"

TEST(ExampleTest, Sample1) {
  EXPECT_EQ(0, 0);
};

// main should not be required if executable is linked with gtest_main
// int main(int argc, char *argv[]) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
