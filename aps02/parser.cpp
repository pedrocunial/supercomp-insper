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

WordTrie *Parser::get_trie(std::string key)
{
  // TODO: This should be in the wordtrie (using increment to parent)
  //       but I'm unsure about it soo I'm keeping it here :^)
  WordTrie *trie = NULL;
  if (this->map.count(key)) {  // map contains key
    trie = this->map[key];
    trie->root->increment();
  } else {
    trie = new WordTrie(key, this->depth);
    this->map[key] = trie;
  }
  return trie;
}

void Parser::run(void)
{
  std::size_t depth = this->depth;
  puts("before tokens");
  std::vector<std::string> tokens = this->tokens;
  puts("after tokens");
  std::size_t limit = tokens.size() - depth;

  for (auto i=0; i<limit; i++) {
    std::string curr_root = tokens[i];

    auto begin = tokens.begin() + i;
    auto end   = tokens.begin() + i + depth;
    std::vector<std::string> tmp_vec(begin, end);
    this->get_trie(curr_root)->add_word(tmp_vec);
  }
}
