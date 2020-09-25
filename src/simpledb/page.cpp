#include "simpledb/page.h"

namespace simpledb {

Page::Page() {
  content = new char[PageSize];
}

Page::~Page() {
  delete[] content;
}

} /* namespace simpledb */
