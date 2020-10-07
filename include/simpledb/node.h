#ifndef SIMPLEDB_NODE_H_INCLUDED
#define SIMPLEDB_NODE_H_INCLUDED

#include "simpledb/page.h"
#include "simpledb/types.h"

#include <cstdint>
#include <iostream>

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
extern const uint32_t LEAF_NODE_NEXT_LEAF_SIZE;
extern const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET;
extern const uint32_t LEAF_NODE_HEADER_SIZE;

// leaf node body layout
extern const uint32_t LEAF_NODE_KEY_SIZE;
extern const uint32_t LEAF_NODE_KEY_OFFSET;
extern const uint32_t LEAF_NODE_VALUE_SIZE;
extern const uint32_t LEAF_NODE_VALUE_OFFSET;
extern const uint32_t LEAF_NODE_CELL_SIZE;
extern const uint32_t LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t LEAF_NODE_MAX_CELLS;

extern const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT;
extern const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT;

// internal node header layout
extern const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE;
extern const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET;
extern const uint32_t INTERNAL_NODE_HEADER_SIZE;

// internal node body layout
extern const uint32_t INTERNAL_NODE_KEY_SIZE;
extern const uint32_t INTERNAL_NODE_CHILD_SIZE;
extern const uint32_t INTERNAL_NODE_CELL_SIZE;
extern const uint32_t INTERNAL_NODE_MAX_CELLS;

class Node {
public:
  Node(char *node);

  void initialize_leaf_node();
  void initialize_internal_node();
  NodeType get_type();
  void set_type(NodeType type);
  bool get_is_root();
  void set_is_root(bool is_root);
  uint32_t get_max_key();
  uint32_t* parent();

  uint32_t* leaf_num_cells();
  uint32_t* leaf_next_leaf();
  char* leaf_cell(uint32_t cell_num);
  uint32_t* leaf_key(uint32_t cell_num);
  char* leaf_value(uint32_t cell_num);

  uint32_t* internal_num_keys();
  uint32_t* internal_right_child();
  uint32_t* internal_cell(uint32_t child_num);
  uint32_t* internal_child(uint32_t num);
  uint32_t* internal_key(uint32_t key);
  void internal_update_key(uint32_t old_max, uint32_t new_max);
  uint32_t internal_find_child(uint32_t key);

  char* node;
};

} /* namespace simpledb */

#endif /* ifndef SIMPLEDB_NODE_H_INCLUDED */
