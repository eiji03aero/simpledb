#ifndef ROW_H_INCLUDED
#define ROW_H_INCLUDED

#include "simpledb/types.h"

#include <cstring>
#include <cstdlib>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

namespace simpledb {

class Row {
public:
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE + 1];
  char email[COLUMN_EMAIL_SIZE + 1];

  void serialize(char *destination);
  void deserialize(char *destination);
};

extern const uint32_t ID_SIZE;
extern const uint32_t USERNAME_SIZE;
extern const uint32_t EMAIL_SIZE;
extern const uint32_t ID_OFFSET;
extern const uint32_t USERNAME_OFFSET;
extern const uint32_t EMAIL_OFFSET;
extern const uint32_t ROW_SIZE;

} /* namespace simpledb */

#endif /* ifndef ROW_H_INCLUDED */
