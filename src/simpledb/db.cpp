#include "simpledb/db.h"

namespace simpledb {

void print_prompt() {
  std::cout << "db > ";
}

int db(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Error: must supply a database filename" << '\n';
    exit(EXIT_FAILURE);
  }

  char *filename = argv[1];

  Table *table { new Table };
  table->db_open(filename);
  InputBuffer input_buffer;

  while (true) {
    std::string input;

    print_prompt();
    std::getline(std::cin, input);

    // input_buffer.write(input);

    if (input.find(".") == 0) {
      switch (do_meta_command(input, table)) {
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
      case (PrepareResult::StringTooLong):
        std::cout << "Error: string is too long" << '\n';
        continue;
      case (PrepareResult::NegativeId):
        std::cout << "Error: id must be positive" << '\n';
        continue;
      case (PrepareResult::SyntaxError):
        std::cout << "Error: could not parse statement" << '\n';
        continue;
      case (PrepareResult::UnrecognizedStatement):
        std::cout << "Error: unrecognized keyword at start of: " << input << '\n';
        continue;
    }

    switch (statement.execute(table)) {
      case (ExecuteResult::Success):
        std::cout << "executed" << '\n';
        break;
      case (ExecuteResult::TableFull):
        std::cout << "Error: table full" << '\n';
        break;
      case (ExecuteResult::UnknownStatement):
        std::cout << "Error: unknown statement" << '\n';
        break;
      case (ExecuteResult::DuplicateKey):
        std::cout << "Error: duplicate key" << '\n';
        break;
    }
  }

  return 0;
}

} /* namespace simpledb */
