#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include <string>
#include "simpledb/types.h"

namespace simpledb {

MetaCommandResult do_meta_command(std::string &input);

} /* namespace simpledb */

#endif /* ifndef COMMAND_H_INCLUDED */
