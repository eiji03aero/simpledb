#include "simpledb/table.h"

namespace simpledb {

Table::Table()
  : root_page_num { 0 },
    pager { new Pager } {}

void Table::db_open(const char *filename) {
  pager->open(filename);

  if (pager->num_pages == 0) {
    Page *root_page = pager->get_page(0);
    Node root_node(root_page->content);
    root_node.initialize_leaf_node();
    root_node.set_is_root(true);
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
  Node node(page->content);
  return node.leaf_value(cursor->cell_num);
}

Cursor* Table::find(uint32_t key) {
  Page *page = pager->get_page(root_page_num);
  Node node(page->content);

  if (node.get_type() == NodeType::Leaf) {
    return find_leaf_node(root_page_num, key);
  } else {
    return find_internal_node(root_page_num, key);
  }
}

Cursor* Table::find_leaf_node(uint32_t page_num, uint32_t key) {
  Page *page = pager->get_page(page_num);
  Node node(page->content);
  uint32_t num_cells = *(node.leaf_num_cells());

  Cursor *cursor = new Cursor(this);
  cursor->page_num = page_num;

  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;
  while (one_past_max_index != min_index) {
    uint32_t index = (min_index + one_past_max_index) / 2;
    uint32_t key_at_index = *(node.leaf_key(index));
    if (key == key_at_index) {
      cursor->cell_num = index;
      return cursor;
    }
    if (key < key_at_index) {
      one_past_max_index = index;
    } else {
      min_index = index + 1;
    }
  }

  cursor->cell_num = min_index;
  return cursor;
}

Cursor* Table::find_internal_node(uint32_t page_num, uint32_t key) {
  Page *page = pager->get_page(page_num);
  Node node(page->content);

  uint32_t child_index = node.internal_find_child(key);
  uint32_t child_num = *(node.internal_child(child_index));

  Page *child_page = pager->get_page(child_num);
  Node child_node(child_page->content);
  switch (child_node.get_type()) {
    case NodeType::Leaf:
      return find_leaf_node(child_num, key);
    case NodeType::Internal:
      return find_internal_node(child_num, key);
    default:
      std::cout << "Error: unknown node type " << static_cast<uint32_t>(child_node.get_type()) << '\n';
      exit(EXIT_FAILURE);
  }
}

void Table::create_new_root(uint32_t right_child_page_num) {
  Page *root_page = pager->get_page(root_page_num);
  Node root_node(root_page->content);

  Page *right_page = pager->get_page(right_child_page_num);
  Node right_child(right_page->content);

  uint32_t left_child_page_num = pager->get_unused_page_num();
  Page *left_page = pager->get_page(left_child_page_num);
  Node left_child(left_page->content);

  memcpy(left_child.node, root_node.node, Page::PageSize);
  left_child.set_is_root(false);

  root_node.initialize_internal_node();
  root_node.set_is_root(true);
  *(root_node.internal_num_keys()) = 1;
  *(root_node.internal_child(0)) = left_child_page_num;
  uint32_t left_child_max_key = left_child.get_max_key();
  *(root_node.internal_key(0)) = left_child_max_key;
  *(root_node.internal_right_child()) = right_child_page_num;
  *(left_child.parent()) = root_page_num;
  *(right_child.parent()) = root_page_num;
}

void Cursor::table_start() {
  Page *page = table->pager->get_page(page_num);
  Node node(page->content);

  uint32_t num_cells = *(node.leaf_num_cells());
  end_of_table = (num_cells == 0);
}

void Cursor::advance() {
  Page *page = table->pager->get_page(page_num);
  Node node(page->content);

  cell_num += 1;
  if (cell_num >= *(node.leaf_num_cells())) {
    uint32_t next_page_num = *(node.leaf_next_leaf());
    if (next_page_num == 0) {
      end_of_table = true;
    } else {
      page_num = next_page_num;
      cell_num = 0;
    }
  }
}

void Cursor::insert_leaf_node(uint32_t key, Row *row) {
  Page *page = table->pager->get_page(page_num);
  Node node(page->content);

  uint32_t num_cells = *(node.leaf_num_cells());
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    split_and_insert_leaf_node(key, row);
    return;
  }

  if (cell_num < num_cells) {
    for (uint32_t i = num_cells; i > cell_num; i--) {
      memcpy(node.leaf_cell(i), node.leaf_cell(i - 1), LEAF_NODE_CELL_SIZE);
    }
  }

  *(node.leaf_num_cells()) += 1;
  *(node.leaf_key(cell_num)) = key;
  row->serialize(node.leaf_value(cell_num));
}

void Cursor::split_and_insert_leaf_node(uint32_t key, Row *row) {
  Page *old_page = table->pager->get_page(page_num);
  Node old_node(old_page->content);
  uint32_t old_max = old_node.get_max_key();

  uint32_t new_page_num = table->pager->get_unused_page_num();
  Page *new_page = table->pager->get_page(new_page_num);
  Node new_node(new_page->content);
  new_node.initialize_leaf_node();


  *(new_node.parent()) = *(old_node.parent());
  *(new_node.leaf_next_leaf()) = *(old_node.leaf_next_leaf());
  *(old_node.leaf_next_leaf()) = new_page_num;

  for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
    char *destination_node_addr;
    if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
      destination_node_addr = new_node.node;
    } else {
      destination_node_addr = old_node.node;
    }
    uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
    Node destination_node(destination_node_addr);
    char *destination = destination_node.leaf_cell(index_within_node);

    if (i == cell_num) {
      row->serialize(destination_node.leaf_value(index_within_node));
      *(destination_node.leaf_key(index_within_node)) = key;
    } else if (i > cell_num) {
      memcpy(destination, old_node.leaf_cell(i - 1), LEAF_NODE_CELL_SIZE);
    } else {
      memcpy(destination, old_node.leaf_cell(i), LEAF_NODE_CELL_SIZE);
    }
  }

  *(old_node.leaf_num_cells()) = LEAF_NODE_LEFT_SPLIT_COUNT;
  *(new_node.leaf_num_cells()) = LEAF_NODE_RIGHT_SPLIT_COUNT;

  if (old_node.get_is_root()) {
    return table->create_new_root(new_page_num);
  } else {
    uint32_t parent_page_num = *(old_node.parent());
    uint32_t new_max = old_node.get_max_key();
    Page *parent_page = table->pager->get_page(parent_page_num);
    Node parent_node(parent_page->content);

    parent_node.internal_update_key(old_max, new_max);
    insert_internal_node(parent_page_num, new_page_num);
  }
}

void Cursor::insert_internal_node(uint32_t parent_page_num, uint32_t child_page_num) {
  Page *parent_page = table->pager->get_page(parent_page_num);
  Node parent_node(parent_page->content);
  Page *child_page = table->pager->get_page(child_page_num);
  Node child_node(child_page->content);

  uint32_t child_max_key = child_node.get_max_key();
  uint32_t index = parent_node.internal_find_child(child_max_key);

  uint32_t original_num_keys = *(parent_node.internal_num_keys());
  *(parent_node.internal_num_keys()) = original_num_keys + 1;

  if (original_num_keys >= INTERNAL_NODE_MAX_CELLS) {
    std::cout << "Error: need to implement splitting internal node" << '\n';
    exit(EXIT_FAILURE);
  }

  uint32_t right_child_page_num = *(parent_node.internal_right_child());
  Page *right_page = table->pager->get_page(right_child_page_num);
  Node right_child(right_page->content);

  if (child_max_key > right_child.get_max_key()) {
    *(parent_node.internal_child(original_num_keys)) = right_child_page_num;
    *(parent_node.internal_key(original_num_keys)) = right_child.get_max_key();
    *(parent_node.internal_right_child()) = child_page_num;
  } else {
    for (uint32_t i = original_num_keys; i > index; i--) {
      char *destination = reinterpret_cast<char*>(parent_node.internal_cell(i));
      char *source = reinterpret_cast<char*>(parent_node.internal_cell(i - 1));
      memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
    }
    *(parent_node.internal_child(index)) = child_page_num;
    *(parent_node.internal_key(index)) = child_max_key;
  }
}


} /* namespace simpledb */
