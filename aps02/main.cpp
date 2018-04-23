#include <iostream>

#include "tokenizer.hpp"

int main(int argc, char **argv)
{
  Tokenizer *tokenizer = new Tokenizer();
  if (argc < 1) {
    std::cout << "[ERR] No input file" << std::endl;
    return 1;
  }

  puts("Loading file...");
  tokenizer->load_file(argv[1]);
  puts("DONE!");

  puts("Generating tokens...");
  auto tokens = tokenizer->get_tokens();
  puts("DONE!");

  delete tokenizer;
  return 0;
}
