#include "simpledb/table.h"

namespace simpledb {

Table::Table()
  : root_page_num { 0 },
    pager { new Pager } {}

void Table::db_open(const char *filename) {
  pager->open(filename);

  if (pager->num_pages == 0) {
    Page *root_page = pager->get_page(0);
    LeafNode root_node(root_page->content);
    root_node.initialize();
  }
}

void Table::db_close() {
  for (uint32_t i {0}; i < pager->num_pages; i++) {
    if (pager->pages[i] == NULL) {
      continue;
    }
    pager->flush(i);
    pager->delete_page(i);
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

char* Table::cursor_value(Cursor *cursor) {
  uint32_t page_num = cursor->page_num;
  Page *page = cursor->table->pager->get_page(page_num);
  LeafNode node(page->content);
  return node.value(cursor->cell_num);
}

void Cursor::table_start() {
  page_num = table->root_page_num;
  cell_num = 0;

  Page *root_page = table->pager->get_page(table->root_page_num);
  LeafNode root_node(root_page->content);
  uint32_t num_cells = *(root_node.num_cells());
  end_of_table = (num_cells == 0);
}

void Cursor::table_end() {
  page_num = table->root_page_num;
  Page *root_page = table->pager->get_page(table->root_page_num);
  LeafNode root_node(root_page->content);
  uint32_t num_cells = *(root_node.num_cells());
  cell_num = num_cells;

  end_of_table = true;
}

void Cursor::advance() {
  Page *page = table->pager->get_page(page_num);
  LeafNode node(page->content);

  cell_num += 1;
  if (cell_num >= *(node.num_cells())) {
    end_of_table = true;
  }
}

void Cursor::insert_leaf_node(uint32_t key, Row *row) {
  Page *page = table->pager->get_page(page_num);
  LeafNode node(page->content);

  uint32_t num_cells = *(node.num_cells());
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    std::cout << "Need to implement splitting a leaf node" << '\n';
    exit(EXIT_FAILURE);
  }

  if (cell_num < num_cells) {
    for (uint32_t i = num_cells; i > cell_num; i--) {
      memcpy(node.cell(i), node.cell(i - 1), LEAF_NODE_CELL_SIZE);
    }
  }

  *(node.num_cells()) += 1;
  *(node.key(cell_num)) = key;
  row->serialize(node.value(cell_num));
}

} /* namespace simpledb */
