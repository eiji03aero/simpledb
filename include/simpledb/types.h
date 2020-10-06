#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

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
  Success,
  TableFull,
  UnknownStatement,
  DuplicateKey
};

enum class NodeType {
  Internal,
  Leaf,
};

} /* namespace simpledb */

#endif /* ifndef TYPE_H_INCLUDED */
