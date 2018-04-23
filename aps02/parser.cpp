#include "parser.hpp"

Parser::Parser(const char *filename, std::size_t depth)
{
  this->tokenizer = new Tokenizer();

  puts("Loading file...");
  this->tokenizer->load_file(filename);
  puts("DONE!");

  puts("Generating tokens...");
  this->tokens = this->tokenizer->get_tokens();
  puts("DONE!");

  // for ngram size (AKA n)
  this->depth = depth;
}

Parser::~Parser(void)
{
  delete this->tokenizer;
}

