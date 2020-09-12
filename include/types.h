#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

enum class MetaCommandResult {
  Success,
  UnregocnizedCommand
};

enum class PrepareResult {
  Success,
  UnrecognizedStatement
};

enum class StatementType {
  Insert,
  Select
};

#endif /* ifndef TYPE_H_INCLUDED */
