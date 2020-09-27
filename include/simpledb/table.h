#ifndef SIMPLEDB_TABLE_H_INCLUDED
#define SIMPLEDB_TABLE_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/row.h"
#include "simpledb/page.h"
#include "simpledb/pager.h"

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
  uint32_t rows_per_page();
  uint32_t max_rows();

  uint32_t num_rows;
  Pager *pager;
};

class Cursor {
public:
  Cursor(Table *tbl)
    : table { tbl } {};

  void table_start();
  void table_end();
  void advance();

  Table *table;
  uint32_t row_num;
  bool end_of_table;
};

} /* namespace simpledb */

#endif /* ifndef SIMPLEDB_TABLE_H_INCLUDED */
