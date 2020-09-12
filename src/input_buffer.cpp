#include "input_buffer.h"

void InputBuffer::write(std::string &input) {
  buffer = input;
  input_length = input.size();
}
