#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/row.h"
#include "simpledb/page.h"

#include <cstring>
#include <cstdlib>

namespace simpledb {

class Table {
public:
  static const uint32_t MaxPages { 100 };

  Table();
  ~Table() = default;

  uint32_t num_rows;
  Page *pages[MaxPages];

  char* row_slot(uint32_t row_num);
  uint32_t rows_per_page();
  uint32_t max_rows();
};

} /* namespace simpledb */

#endif /* ifndef TABLE_H_INCLUDED */
