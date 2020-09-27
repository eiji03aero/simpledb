#ifndef SIMPLEDB_PAGER_H_INCLUDED
#define SIMPLEDB_PAGER_H_INCLUDED

#include "simpledb/types.h"
#include "simpledb/page.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>

namespace simpledb {

class Pager {
public:
  void open(const char* filename);
  Page* get_page(uint32_t page_num);
  void delete_page(uint32_t page_num);
  void flush(uint32_t page_num, uint32_t bytes_to_flush);

  static const uint32_t MaxPages { 100 };

  int file_descriptor;
  uint32_t file_length;
  Page *pages[MaxPages];
};

} /* namespace simpledb */

#endif /* ifndef SIMPLEDB_PAGER_H_INCLUDED */
