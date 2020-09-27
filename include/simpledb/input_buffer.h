#ifndef INPUT_BUFFER_H_INCLUDED
#define INPUT_BUFFER_H_INCLUDED

#include <string>

namespace simpledb {

class InputBuffer {
public:
  void write(std::string &input);

  std::string buffer;
  std::size_t buffer_length{0};
  std::size_t input_length{0};
};

} /* namespace simpledb */

#endif /* ifndef INPUT_BUFFER_H_INCLUDED */
