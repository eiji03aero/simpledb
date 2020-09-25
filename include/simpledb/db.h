#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/input_buffer.h"
#include "simpledb/command.h"
#include "simpledb/statement.h"
#include "simpledb/table.h"

#include <cstddef>
#include <cstdio>
#include <string>
#include <iostream>

namespace simpledb {

void print_prompt();

int db(int argc, char* argv[]);

} /* namespace simpledb */

#endif /* ifndef DB_H_INCLUDED */
