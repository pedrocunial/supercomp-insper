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

  void run(void);
  Parser(const char *filename, std::size_t depth);
  ~Parser(void);

private:
  std::size_t depth;
  Tokenizer *tokenizer;
  std::vector<std::string> tokens;

  WordTrie *get_trie(std::string key);
};

#endif
