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

void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    std::cout << "  ";
  }
}

void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level) {
  Page *page = pager->get_page(page_num);
  Node node(page->content);
  uint32_t num_keys, child;

  switch (node.get_type()){
    case (NodeType::Leaf):
      num_keys = *(node.leaf_num_cells());
      indent(indentation_level);
      std::cout << "- leaf (size " << num_keys << ")" << '\n';
      for (uint32_t i = 0; i < num_keys; i++) {
        indent(indentation_level + 1);
        std::cout << "- " << *(node.leaf_key(i)) << '\n';
      }
      break;
    case (NodeType::Internal):
      num_keys = *(node.internal_num_keys());
      indent(indentation_level);
      std::cout << "- internal (size " << num_keys << ")" << '\n';
      for (uint32_t i = 0; i < num_keys; i++) {
        child = *(node.internal_child(i));
        print_tree(pager, child, indentation_level + 1);
        indent(indentation_level + 1);
        std::cout << "- key " << *(node.internal_key(i)) << '\n';
      }
      child = *(node.internal_right_child());
      print_tree(pager, child, indentation_level + 1);
      break;
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
    Node node(page->content);
    print_tree(table->pager, 0, 0);
    return MetaCommandResult::Success;
  } else {
    return MetaCommandResult::UnregocnizedCommand;
  }
}

} /* namespace simpledb */
