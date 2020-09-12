#ifndef STATEMENT_H_INCLUDED
#define STATEMENT_H_INCLUDED

#include <string>
#include <iostream>
#include "types.h"

class Statement {
public:
  StatementType type;

  PrepareResult prepare(std::string &input);
  void execute();
};

#endif /* ifndef STATEMENT_H_INCLUDED */
