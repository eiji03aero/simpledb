#include "statement.h"

PrepareResult Statement::prepare(std::string &input) {
  if (input.find("insert") == 0) {
    type = StatementType::Insert;
    return PrepareResult::Success;
  }
  if (input.find("select") == 0) {
    type = StatementType::Select;
    return PrepareResult::Success;
  }

  return PrepareResult::UnrecognizedStatement;
}

void Statement::execute() {
  switch (type) {
    case (StatementType::Insert):
      std::cout << "gonna do insert here" << '\n';
      break;
    case (StatementType::Select):
      std::cout << "gonna do select here" << '\n';
      break;
  }
}
