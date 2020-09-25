#include "simpledb/row.h"

namespace simpledb {

extern const uint32_t ID_SIZE { size_of_attribute(Row, id) };
extern const uint32_t USERNAME_SIZE = { size_of_attribute(Row, username) };
extern const uint32_t EMAIL_SIZE = { size_of_attribute(Row, email) };
extern const uint32_t ID_OFFSET = { 0 };
extern const uint32_t USERNAME_OFFSET = { ID_OFFSET + ID_SIZE };
extern const uint32_t EMAIL_OFFSET = { USERNAME_OFFSET + USERNAME_SIZE };
extern const uint32_t ROW_SIZE = { ID_SIZE + USERNAME_SIZE + EMAIL_SIZE };

void Row::serialize(char *destination) {
  std::memcpy((destination + ID_OFFSET), &(this->id), ID_SIZE);
  std::memcpy((destination + USERNAME_OFFSET), &(this->username), USERNAME_SIZE);
  std::memcpy((destination + EMAIL_OFFSET), &(this->email), EMAIL_SIZE);
}

void Row::deserialize(char *destination) {
  std::memcpy(&(this->id), (destination + ID_OFFSET), ID_SIZE);
  std::memcpy(&(this->username), (destination + USERNAME_OFFSET), USERNAME_SIZE);
  std::memcpy(&(this->email), (destination + EMAIL_OFFSET), EMAIL_SIZE);
}

} /* namespace simpledb */
