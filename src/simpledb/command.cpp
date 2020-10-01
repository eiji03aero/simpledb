#include "simpledb/command.h"

namespace simpledb {

void print_constants() {
  std::cout << "ROW_SIZE: " << ROW_SIZE << '\n';
  std::cout << "COMMON_NODE_HEADER_SIZE: " << COMMON_NODE_HEADER_SIZE << '\n';
  std::cout << "LEAF_NODE_HEADER_SIZE: " << LEAF_NODE_HEADER_SIZE << '\n';
  std::cout << "LEAF_NODE_CELL_SIZE: " << LEAF_NODE_CELL_SIZE << '\n';
  std::cout << "LEAF_NODE_SPACE_FOR_CELLS: " << LEAF_NODE_SPACE_FOR_CELLS << '\n';
  std::cout << "LEAF_NODE_MAX_CELLS: " << LEAF_NODE_MAX_CELLS << '\n';
}

void print_leaf_node(LeafNode node) {
  uint32_t num_cells = *(node.num_cells());
  std::cout << "leaf (size: " << num_cells << ")" << '\n';
  for (uint32_t i = 0; i < num_cells; i++) {
    uint32_t key = *(node.key(i));
    std::cout << "  - " << i << " : " << key << '\n';
  }
}

MetaCommandResult do_meta_command(std::string &input, Table *table) {
  if (input == ".exit") {
    table->db_close();
    exit(EXIT_SUCCESS);
  } else if (input == ".constants") {
    std::cout << "Constants: " << '\n';
    print_constants();
    return MetaCommandResult::Success;
  } else if (input == ".btree") {
    std::cout << "Tree: " << '\n';
    Page *page = table->pager->get_page(0);
    LeafNode node(page->content);
    print_leaf_node(node);
    return MetaCommandResult::Success;
  } else {
    return MetaCommandResult::UnregocnizedCommand;
  }
}

} /* namespace simpledb */
