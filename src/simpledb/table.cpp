#include "simpledb/table.h"

namespace simpledb {

Table::Table()
  : num_rows { 0 } {}

char* Table::row_slot(uint32_t row_num) {
  uint32_t page_num = row_num / rows_per_page();
  Page *page = pages[page_num];
  if (page == NULL) {
    page = pages[page_num] = new Page;
  }
  uint32_t row_offset = row_num % rows_per_page();
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page->content + byte_offset;
}

uint32_t Table::rows_per_page() {
  return Page::PageSize / ROW_SIZE;
}

uint32_t Table::max_rows() {
  return rows_per_page() * MaxPages;
}

} /* namespace simpledb */
