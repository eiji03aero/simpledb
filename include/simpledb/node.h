#ifndef SIMPLEDB_NODE_H_INCLUDED
#define SIMPLEDB_NODE_H_INCLUDED

#include <cstdint>

namespace simpledb {

// common node header layout
extern const uint32_t NODE_TYPE_SIZE;
extern const uint32_t NODE_TYPE_OFFSET;
extern const uint32_t IS_ROOT_SIZE;
extern const uint32_t IS_ROOT_OFFSET;
extern const uint32_t PARENT_POINTER_SIZE;
extern const uint32_t PARENT_POINTER_OFFSET;
extern const uint32_t COMMON_NODE_HEADER_SIZE;

// leaf node header layout
extern const uint32_t LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

// leaf node body layout
extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;
extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;

class LeafNode {
public:
  LeafNode(char *node);

  void initialize();
  uint32_t* num_cells();
  char* cell(uint32_t cell_num);
  char* key(uint32_t cell_num);
  char* value(uint32_t cell_num);

  char* node;
};

} /* namespace simpledb */

#endif /* ifndef SIMPLEDB_NODE_H_INCLUDED */
