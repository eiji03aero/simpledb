#ifndef SIMPLEDB_TABLE_H_INCLUDED
#define SIMPLEDB_TABLE_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/row.h"
#include "simpledb/page.h"
#include "simpledb/pager.h"
#include "simpledb/node.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <iostream>

namespace simpledb {

class Cursor;

class Table {
public:
  Table();
  ~Table() = default;

  void db_open(const char *filename);
  void db_close();
  char* cursor_value(Cursor *cursor);
  Cursor* find(uint32_t key_to_insert);
  Cursor* find_leaf_node(uint32_t page_num, uint32_t key_to_insert);
  void create_new_root(uint32_t right_child_page_num);

  uint32_t root_page_num;
  Pager *pager;
};

class Cursor {
public:
  Cursor(Table *tbl)
    : table { tbl } {};

  void table_start();
  void advance();
  void insert_leaf_node(uint32_t key, Row *row);
  void split_and_insert_leaf_node(uint32_t key, Row *row);

  Table *table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table;
};

} /* namespace simpledb */

#endif /* ifndef SIMPLEDB_TABLE_H_INCLUDED */
