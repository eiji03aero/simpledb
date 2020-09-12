#include <cstddef>
#include <cstdio>
#include <string>
#include <iostream>

#include "input_buffer.h"

void print_prompt()
{
  printf("db > ");
}

int main(int argc, char* argv[])
{
  InputBuffer input_buffer;

  while (true)
  {
    std::string input;

    print_prompt();
    std::cin >> input;

    input_buffer.write(input);

    if (input == ".exit")
    {
      exit(EXIT_SUCCESS);
    }
    else
    {
      std::cout << "unrecognized command: " << input << '\n';
    }
  }

  return 0;
}
