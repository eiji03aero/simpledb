#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#include <cstdint>

namespace simpledb {

enum class MetaCommandResult {
  Success,
  UnregocnizedCommand
};

enum class PrepareResult {
  Success,
  UnrecognizedStatement,
  SyntaxError,
  StringTooLong,
  NegativeId
};

enum class StatementType {
  Insert,
  Select
};

enum class ExecuteResult {
  TableFull,
  Success,
  UnknownStatement
};

} /* namespace simpledb */

#endif /* ifndef TYPE_H_INCLUDED */
