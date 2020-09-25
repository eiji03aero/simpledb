#include <future>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>

#include "gtest/gtest.h"

TEST(ExampleTest, Defered1) {
  bool hoge { true };

  std::thread {[&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds {100});
    hoge = false;
  }}.detach();

  EXPECT_TRUE(hoge);
  std::this_thread::sleep_for(std::chrono::milliseconds {120});
  EXPECT_FALSE(hoge);
};
