#include <string>
#include "command.h"

MetaCommandResult do_meta_command(std::string &input) {
  if (input == ".exit") {
    exit(EXIT_SUCCESS);
  } else {
    return MetaCommandResult::UnregocnizedCommand;
  }
}
