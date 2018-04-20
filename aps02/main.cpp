#include <iostream>

#include "tokenizer.hpp"

int main(int argc, char **argv)
{
  std::unique_ptr<Tokenizer> tokenizer;
  if (argc < 1) {
    std::cout << "[ERR] No input file" << std::endl;
    return 1;
  }
  std::cout << "zapzap" << std::endl;
  tokenizer->load_file(argv[1]);
  std::cout << "zapzap" << std::endl;
  std::cout << tokenizer->text << std::endl;
  return 0;
}
