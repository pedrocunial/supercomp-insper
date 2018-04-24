#include <iostream>

#include "parser.hpp"

int main(int argc, char **argv)
{
  if (argc < 1) {
    std::cout << "[ERR] No input file" << std::endl;
    return 1;
  }
  Parser *parser = new Parser(argv[1], 3);
  parser->run();

  puts("Generating text...");
  std::string text = parser->generate_text(22);
  puts("Generated text:");
  std::cout << text << std::endl;

  delete parser;
  return 0;
}
