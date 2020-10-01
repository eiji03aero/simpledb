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
  Page *page = table->pager->get_page(table->root_page_num);
  LeafNode node(page->content);
  if (*(node.num_cells()) >= LEAF_NODE_MAX_CELLS) {
    return ExecuteResult::TableFull;
  }

  Cursor cursor(table);
  cursor.table_end();
  cursor.insert_leaf_node(row_to_insert.id, &row_to_insert);

  return ExecuteResult::Success;
}

void print_row(Row *row) {
  std::cout << row->id << " " << row->username << " " << row->email << '\n';
}

ExecuteResult Statement::execute_select(Table *table) {
  Cursor cursor(table);
  cursor.table_start();

  Row row;
  while (!(cursor.end_of_table)) {
    row.deserialize(table->cursor_value(&cursor));
    print_row(&row);
    cursor.advance();
  }

  return ExecuteResult::Success;
}

} /* namespace simpledb */
