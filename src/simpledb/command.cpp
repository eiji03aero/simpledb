#include "simpledb/command.h"

#include <string>

namespace simpledb {

MetaCommandResult do_meta_command(std::string &input, Table *table) {
  if (input == ".exit") {
    table->db_close();
    exit(EXIT_SUCCESS);
  } else {
    return MetaCommandResult::UnregocnizedCommand;
  }
}

} /* namespace simpledb */
