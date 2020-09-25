#include "simpledb/statement.h"

namespace simpledb {

PrepareResult Statement::prepare(std::string &input) {
  if (input.find("insert") == 0) {
    return prepare_insert(input);
  }
  if (input.find("select") == 0) {
    type = StatementType::Select;
    return PrepareResult::Success;
  }

  return PrepareResult::UnrecognizedStatement;
}

PrepareResult Statement::prepare_insert(std::string &input) {
  type = StatementType::Insert;

  std::stringstream ss { input };
  std::string keyword;
  std::string id_str;
  std::string name;
  std::string email;
  std::getline(ss, keyword, ' ');
  std::getline(ss, id_str, ' ');
  std::getline(ss, name, ' ');
  std::getline(ss, email);

  if (id_str.empty() || name.empty() || email.empty()) {
    return PrepareResult::SyntaxError;
  }

  int id { atoi(id_str.c_str()) };
  if (id < 0) {
    return PrepareResult::NegativeId;
  }
  if (
    name.size() > COLUMN_USERNAME_SIZE
    || email.size() > COLUMN_EMAIL_SIZE
  ) {
    return PrepareResult::StringTooLong;
  }

  row_to_insert.id = id;
  strcpy(row_to_insert.username, name.c_str());
  strcpy(row_to_insert.email, email.c_str());

  return PrepareResult::Success;
}

ExecuteResult Statement::execute(Table *table) {
  switch (type) {
    case (StatementType::Insert):
      return execute_insert(table);
    case (StatementType::Select):
      return execute_select(table);
  }

  return ExecuteResult::UnknownStatement;
}

ExecuteResult Statement::execute_insert(Table *table) {
  if (table->num_rows >= table->max_rows()) {
    return ExecuteResult::TableFull;
  }

  row_to_insert.serialize(table->row_slot(table->num_rows));
  table->num_rows += 1;

  return ExecuteResult::Success;
}

void print_row(Row *row) {
  std::cout << row->id << " " << row->username << " " << row->email << '\n';
}

ExecuteResult Statement::execute_select(Table *table) {
  Row row;
  for (uint32_t i = 0; i < table->num_rows; i++) {
    row.deserialize(table->row_slot(i));
    print_row(&row);
  }

  return ExecuteResult::Success;
}

} /* namespace simpledb */
