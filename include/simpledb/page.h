#ifndef PAGE_H_INCLUDED
#define PAGE_H_INCLUDED

#include "simpledb/types.h"

namespace simpledb {

class Page {
public:
  static const uint32_t PageSize { 4096 };

  char *content;

  Page();
  ~Page();
};

} /* namespace simpledb */

#endif /* ifndef PAGE_H_INCLUDED */
