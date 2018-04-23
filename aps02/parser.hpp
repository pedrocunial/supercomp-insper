#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <vector>

#include "tokenizer.hpp"
#include "wordtrie/wordtrie.hpp"

class Parser
{
public:
  Parser(const char *filename, std::size_t depth);
  ~Parser(void);

private:
  Tokenizer *tokenizer;
  std::size_t depth;
  std::vector<std::string> tokens;
};

#endif
