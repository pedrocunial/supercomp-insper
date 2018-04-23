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

  delete parser;
  return 0;
}
