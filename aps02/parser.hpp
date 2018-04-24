#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <random>
#include <string>
#include <vector>
#include <map>

#include "tokenizer.hpp"
#include "wordtrie/wordtrie.hpp"

class Parser
{
public:
  std::map<std::string, WordTrie*> map;
  std::default_random_engine generator;

  void run(void);
  std::string generate_text(std::size_t length);
  Parser(const char *filename, std::size_t depth);
  ~Parser(void);

private:
  std::size_t depth;
  Tokenizer *tokenizer;
  std::vector<std::string> tokens;

  std::string get_randomic_word(std::vector<std::string> &key,
                                std::size_t depth);
  std::string get_randomic_first_word(void);
  WordTrie *get_trie(std::string &key);
};

#endif
