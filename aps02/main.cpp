#include <iostream>
#include <cstdlib>

#include "parser.hpp"

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cout << "[ERR] No input file" << std::endl;
    return 1;
  }
  Parser *parser = new Parser(argv[1], std::strtoul(argv[2], nullptr, 0));
  parser->run();

  puts("Generating text...");
  std::string text = parser->generate_text(std::strtoul(argv[3], nullptr, 0));
  puts("DONE!\nGenerated text:");
  std::cout << text << std::endl;

  delete parser;
  return 0;
}
