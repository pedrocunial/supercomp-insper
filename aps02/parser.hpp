#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <random>
#include <string>
#include <vector>
#include <map>
#include <boost/mpi.hpp>

#include "tokenizer.hpp"
#include "wordtrie/wordtrie.hpp"

#define SELECTED_WORD_TAG    16
#define SELECTED_PERCENT_TAG 17
#define FINAL_WORD           18

class Parser
{
public:
  std::size_t rank, size;
  std::map<std::string, WordTrie*> map;
  std::default_random_engine generator;
  boost::mpi::communicator world;

  void run(void);
  std::string generate_text(std::size_t length);
  Parser(const char *filename, std::size_t depth);
  ~Parser(void);

private:
  std::size_t depth;
  Tokenizer *tokenizer;
  std::vector<std::string> tokens;

  void send_word(const std::string &word, const double proba);
  std::string gather_words(const std::string &word, const double percent);
  std::string select_word(const std::vector<std::string> &words,
                          const std::vector<double> &percent);
  std::string get_randomic_word(std::vector<std::string> &key,
                                std::size_t depth);
  std::string get_randomic_first_word(void);
  WordTrie *get_trie(std::string &key);
};

#endif
