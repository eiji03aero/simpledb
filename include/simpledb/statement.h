#ifndef STATEMENT_H_INCLUDED
#define STATEMENT_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/row.h"
#include "simpledb/table.h"

#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

namespace simpledb {

class Statement {
public:
  StatementType type;
  Row row_to_insert;

  PrepareResult prepare(std::string &input);
  PrepareResult prepare_insert(std::string &input);
  ExecuteResult execute(Table* table);
  ExecuteResult execute_insert(Table* table);
  ExecuteResult execute_select(Table* table);
};

} /* namespace simpledb */

#endif /* ifndef STATEMENT_H_INCLUDED */
