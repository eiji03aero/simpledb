#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/table.h"
#include "simpledb/node.h"

#include <string>
#include <iostream>

namespace simpledb {

MetaCommandResult do_meta_command(std::string &input, Table *table);

} /* namespace simpledb */

#endif /* ifndef COMMAND_H_INCLUDED */
