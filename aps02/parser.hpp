#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <string>
#include <vector>
#include <map>

#include "tokenizer.hpp"
#include "wordtrie/wordtrie.hpp"

class Parser
{
public:
  std::map<std::string, WordTrie*> map;

  Parser(const char *filename, std::size_t depth);
  ~Parser(void);

  void run(void);

private:
  Tokenizer *tokenizer;
  std::size_t depth;
  std::vector<std::string> tokens;

  WordTrie *get_trie(std::string key);
};

#endif
