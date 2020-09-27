#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/row.h"
#include "simpledb/page.h"
#include "simpledb/pager.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

namespace simpledb {

class Table {
public:
  Table();
  ~Table() = default;

  uint32_t num_rows;
  Pager *pager;

  void db_open(const char *filename);
  void db_close();
  char* row_slot(uint32_t row_num);
  uint32_t rows_per_page();
  uint32_t max_rows();
};

} /* namespace simpledb */

#endif /* ifndef TABLE_H_INCLUDED */
