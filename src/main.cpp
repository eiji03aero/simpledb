#include <cstddef>
#include <cstdio>
#include <string>
#include <iostream>

#include "types.h"
#include "input_buffer.h"
#include "command.h"
#include "statement.h"

void print_prompt() {
  printf("db > ");
}

int main(int argc, char* argv[]) {
  InputBuffer input_buffer;

  while (true) {
    std::string input;

    print_prompt();
    std::getline(std::cin, input);

    input_buffer.write(input);

    if (input.find(".") == 0) {
      switch (do_meta_command(input)) {
        case (MetaCommandResult::Success):
          continue;
        case (MetaCommandResult::UnregocnizedCommand):
          std::cout << "unrecognized command: " << input << '\n';
          continue;
      }
      exit(EXIT_SUCCESS);
    }

    Statement statement;
    switch (statement.prepare(input)) {
      case (PrepareResult::Success):
        break;
      case (PrepareResult::UnrecognizedStatement):
        std::cout << "unrecognized keyword at start of: " << input << '\n';
        continue;
    }

    statement.execute();
    std::cout << "executed" << '\n';
  }

  return 0;
}
