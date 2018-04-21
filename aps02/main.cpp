#include <iostream>

#include "tokenizer.hpp"

int main(int argc, char **argv)
{
  Tokenizer *tokenizer = new Tokenizer();
  if (argc < 1) {
    std::cout << "[ERR] No input file" << std::endl;
    return 1;
  }
  std::cout << "zapzap" << std::endl;
  tokenizer->load_file(argv[1]);
  std::cout << "zapzap" << std::endl;
  delete tokenizer;
  return 0;
}
