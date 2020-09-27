#ifndef PAGE_H_INCLUDED
#define PAGE_H_INCLUDED

#include "simpledb/types.h"

#include <cstdint>

namespace simpledb {

class Page {
public:
  Page();
  ~Page();

  static const uint32_t PageSize { 4096 };
  char *content;
};

} /* namespace simpledb */

#endif /* ifndef PAGE_H_INCLUDED */
