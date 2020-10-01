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

LeafNode::LeafNode(char *node)
  : node { node } {};

void LeafNode::initialize() {
  *(num_cells()) = 0;
}

uint32_t* LeafNode::num_cells() {
  return reinterpret_cast<uint32_t*>(node + LEAF_NODE_NUM_CELLS_OFFSET);
}

char* LeafNode::cell(uint32_t cell_num) {
  return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

char* LeafNode::key(uint32_t cell_num) {
  return cell(cell_num);
}

char* LeafNode::value(uint32_t cell_num) {
  return cell(cell_num) + LEAF_NODE_KEY_SIZE;
}

} /* namespace simpledb */
