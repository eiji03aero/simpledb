#include "simpledb/table.h"

namespace simpledb {

Table::Table()
  : num_rows { 0 },
    pager { new Pager } {}

void Table::db_open(const char *filename) {
  pager->open(filename);
  num_rows = pager->file_length / ROW_SIZE;
}

void Table::db_close() {
  uint32_t num_full_pages { num_rows / rows_per_page() };

  for (uint32_t i {0}; i < num_full_pages; i++) {
    if (pager->pages[i] == NULL) {
      continue;
    }
    pager->flush(i, Page::PageSize);
    pager->delete_page(i);
  }

  uint32_t num_additional_rows { num_rows % rows_per_page() };
  if (num_additional_rows > 0) {
    uint32_t page_num { num_full_pages };
    if (pager->pages[page_num] != NULL) {
      pager->flush(page_num, num_additional_rows * ROW_SIZE);
      pager->delete_page(page_num);
    }
  }

  int result = close(pager->file_descriptor);
  if (result == -1) {
    std::cout << "Error: closing db file" << '\n';
    exit(EXIT_FAILURE);
  }

  for (uint32_t i {0}; i < Pager::MaxPages; i++) {
    Page *page { pager->pages[i] };
    if (page) {
      pager->delete_page(i);
    }
  }

  delete pager;
}

char* Table::row_slot(uint32_t row_num) {
  uint32_t page_num = row_num / rows_per_page();
  Page *page = pager->get_page(page_num);
  uint32_t row_offset = row_num % rows_per_page();
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page->content + byte_offset;
}

uint32_t Table::rows_per_page() {
  return Page::PageSize / ROW_SIZE;
}

uint32_t Table::max_rows() {
  return rows_per_page() * Pager::MaxPages;
}

} /* namespace simpledb */
