#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

#include "gtest/gtest.h"

TEST(ExampleTest, Defered1) {
  int status;
  pid_t pid { fork() };

  switch (pid) {
    case -1:
      std::cout << "shippai" << '\n';
      break;
    case 0:
      execl("/bin/echo", "domo");
      std::cout << "executed" << '\n';
      break;
    default:
      std::cout << "child id: " << pid << '\n';
      if (waitpid(pid, &status, 0) != -1) {
        std::cout << "child exited with status: " << pid << '\n';
      } else {
        std::cout << "waitpid" << '\n';
      }
  }
};
