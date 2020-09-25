#include "simpledb/input_buffer.h"

namespace simpledb {

void InputBuffer::write(std::string &input) {
  buffer = input;
  input_length = input.size();
}

} /* namespace simpledb */
