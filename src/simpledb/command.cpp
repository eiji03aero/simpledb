#include "simpledb/command.h"

#include <string>

namespace simpledb {

MetaCommandResult do_meta_command(std::string &input) {
  if (input == ".exit") {
    exit(EXIT_SUCCESS);
  } else {
    return MetaCommandResult::UnregocnizedCommand;
  }
}

} /* namespace simpledb */
