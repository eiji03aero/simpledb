#ifndef INPUT_BUFFER_H_INCLUDED
#define INPUT_BUFFER_H_INCLUDED

#include <string>

class InputBuffer
{
public:
  std::string buffer;
  std::size_t buffer_length{0};
  std::size_t input_length{0};

  void write(std::string &input);
};

#endif /* ifndef INPUT_BUFFER_H_INCLUDED */
