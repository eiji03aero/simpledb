#include "simpledb/node.h"
#include "simpledb/row.h"
#include "simpledb/page.h"

namespace simpledb {

// common node header layout
extern const uint32_t NODE_TYPE_SIZE { sizeof(uint8_t) };
extern const uint32_t NODE_TYPE_OFFSET { 0 };
extern const uint32_t IS_ROOT_SIZE { sizeof(uint8_t) };
extern const uint32_t IS_ROOT_OFFSET { NODE_TYPE_SIZE };
extern const uint32_t PARENT_POINTER_SIZE { sizeof(uint32_t) };
extern const uint32_t PARENT_POINTER_OFFSET { IS_ROOT_OFFSET + IS_ROOT_SIZE };
extern const uint32_t COMMON_NODE_HEADER_SIZE { NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE };

// leaf node header layout
extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE { sizeof(uint32_t) };
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET { COMMON_NODE_HEADER_SIZE };
extern const uint32_t LEAF_NODE_HEADER_SIZE { COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE };

// leaf node body layout
extern const uint32_t LEAF_NODE_KEY_SIZE { sizeof(uint32_t) };
extern const uint32_t LEAF_NODE_KEY_OFFSET { 0 };
extern const uint32_t LEAF_NODE_VALUE_SIZE { ROW_SIZE };
extern const uint32_t LEAF_NODE_VALUE_OFFSET { LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE };
extern const uint32_t LEAF_NODE_CELL_SIZE { LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE };
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS { Page::PageSize - LEAF_NODE_HEADER_SIZE };
extern const uint32_t LEAF_NODE_MAX_CELLS { LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE };

extern const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT { (LEAF_NODE_MAX_CELLS + 1) / 2 };
extern const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT { (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT };

// internal node header layout
extern const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
extern const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET =
  INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
extern const uint32_t INTERNAL_NODE_HEADER_SIZE =
  COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE;

// internal node body layout
extern const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
extern const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
extern const uint32_t INTERNAL_NODE_CELL_SIZE =
  INTERNAL_NODE_KEY_SIZE + INTERNAL_NODE_CHILD_SIZE;

Node::Node(char *node)
  : node { node } {};

void Node::initialize_leaf_node() {
  set_type(NodeType::Leaf);
  set_is_root(false);
  *(leaf_num_cells()) = 0;
}

void Node::initialize_internal_node() {
  set_type(NodeType::Internal);
  set_is_root(false);
  *(internal_num_keys()) = 0;
}

NodeType Node::get_type() {
  uint8_t value = *(reinterpret_cast<uint8_t*>(node + NODE_TYPE_OFFSET));
  return static_cast<NodeType>(value);
}

void Node::set_type(NodeType type) {
  uint8_t value = static_cast<uint8_t>(type);
  *(reinterpret_cast<uint8_t*>(node + NODE_TYPE_OFFSET)) = value;
}

bool Node::get_is_root() {
  uint8_t value = *(reinterpret_cast<uint8_t*>(node + IS_ROOT_OFFSET));
  return static_cast<bool>(value);
}

void Node::set_is_root(bool is_root) {
  uint8_t value = is_root;
  *(reinterpret_cast<uint8_t*>(node + IS_ROOT_OFFSET)) = value;
}

uint32_t Node::get_max_key() {
  switch (get_type()) {
    case NodeType::Internal:
      return *(internal_key(*(internal_num_keys()) - 1));
    case NodeType::Leaf:
      return *(leaf_key(*(leaf_num_cells()) - 1));
    default:
      std::cout << "Unexpected node type" << '\n';
      exit(EXIT_FAILURE);
  }
}

uint32_t* Node::leaf_num_cells() {
  return reinterpret_cast<uint32_t*>(node + LEAF_NODE_NUM_CELLS_OFFSET);
}

char* Node::leaf_cell(uint32_t cell_num) {
  return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t* Node::leaf_key(uint32_t cell_num) {
  return reinterpret_cast<uint32_t*>(leaf_cell(cell_num));
}

char* Node::leaf_value(uint32_t cell_num) {
  return leaf_cell(cell_num) + LEAF_NODE_KEY_SIZE;
}

uint32_t* Node::internal_num_keys() {
  return reinterpret_cast<uint32_t*>(node + INTERNAL_NODE_NUM_KEYS_OFFSET);
}

uint32_t* Node::internal_right_child() {
  return reinterpret_cast<uint32_t*>(node + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

uint32_t* Node::internal_cell(uint32_t cell_num) {
  return reinterpret_cast<uint32_t*>(node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE);
}

uint32_t* Node::internal_child(uint32_t child_num) {
  uint32_t num_keys = *(internal_num_keys());
  if (child_num > num_keys) {
    std::cout << "Error: tried to access child_num " << child_num << " > num_keys " << num_keys << '\n';
    exit(EXIT_FAILURE);
  } else if (child_num == num_keys) {
    return internal_right_child();
  } else {
    return internal_cell(child_num);
  }
}

uint32_t* Node::internal_key(uint32_t key_num) {
  return internal_cell(key_num) + INTERNAL_NODE_CHILD_SIZE;
}

} /* namespace simpledb */
