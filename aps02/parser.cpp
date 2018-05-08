#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <boost/mpi.hpp>

#include "parser.hpp"

#define MASTER 0

#ifdef DEBUG
inline void print_text(std::vector<std::string> &text)
{
  std::cout << "TEXT:";
  for (auto word : text) {
    std::cout << " " << word;
  }
  std::cout << std::endl;
}
#endif

Parser::Parser(const char *filename, std::size_t depth)
{
  boost::mpi::communicator world;

  this->tokenizer = new Tokenizer();

  this->rank = world.rank();
  this->size = world.size();

  #ifdef DEBUG
  std::cout << "rank/size: " << this->rank << "/" << this->size << std::endl;
  #endif

  puts("Loading file...");
  this->tokenizer->load_file(filename);
  puts("DONE!");

  puts("Generating tokens...");
  this->tokens = this->tokenizer->get_tokens();
  puts("DONE!");

  // for ngram size (AKA n)
  this->depth = depth;

  // random seed (for the prediction)
  std::srand(std::time(nullptr));
}

Parser::~Parser(void)
{
  delete this->tokenizer;
}

WordTrie *Parser::get_trie(std::string &key)
{
  // TODO: This should be in the wordtrie (using increment to parent)
  //       but I'm unsure about it soo I'm keeping it here :^)
  WordTrie *trie = NULL;
  if (this->map.count(key)) {  // map contains key
    trie = this->map[key];
  } else {
    trie = new WordTrie(key, this->depth);
    this->map[key] = trie;
  }
  return trie;
}

void Parser::run(void)
{
  // extracting to local variables (faster access (?))
  std::size_t depth = this->depth;
  std::vector<std::string> tokens = this->tokens;
  std::size_t limit = tokens.size() - depth;

  for (std::size_t i=0; i<limit; i++) {
    std::string curr_root = tokens[i];

    auto begin = tokens.begin() + i + 1;
    auto end   = tokens.begin() + i + depth;
    std::vector<std::string> tmp_vec(begin, end);

    WordTrie *trie = this->get_trie(curr_root);
    trie->add_word(tmp_vec);
  }
}

std::string Parser::get_randomic_first_word(void)
{
  // The tokens vector contains repeated words from the text. By getting a
  // randomic value in it, the value itself should already be a "pondered
  // random value"
#ifdef DEBUG
  puts("generating first word!");
  for (auto token : this->tokens) {
    std::cout << token << std::endl;
  }
#endif
  const std::size_t rand_max = this->tokens.size() - this->depth;
  return this->tokens[std::rand() % rand_max];
}

std::string Parser::get_randomic_word(std::vector<std::string> &key,
                                      std::size_t depth)
{
  // Parses to the "depth"th node, and threats it as the leaf
  // uses a discrete distribution to do the "weighted" part of it
#ifdef DEBUG
  std::cout << "ROOT_KEY:" << key[0] << std::endl;
#endif
  WordTrie *trie = this->map[key[0]];

#ifdef DEBUG
  puts("got trie");
#endif

  Node *node  = NULL;
  if (depth == 1) {
    node = trie->root;
#ifdef DEBUG
    puts("got root!");
#endif
  } else {
    auto begin = key.begin() + 1;
    auto end   = key.begin() + depth;
    std::vector<std::string> tmp_vec(begin, end);
    node = trie->get_node(tmp_vec, depth);
#ifdef DEBUG
    puts("got node!");
    std::cout << node->word << std::endl;
#endif
  }

  // words + counts found in node children
  std::vector<std::string> keys;
  std::vector<std::size_t> counts;
#ifdef DEBUG
  puts("before generating counts and keys");
  std::cout << (node->children.empty() ? "empty map" : "map is not empty!")
            << std::endl;
#endif
  for (auto const &it : node->children) {
#ifdef DEBUG
    puts("iteration loop");
    std::cout << "it.first = " << it.first << std::endl;
#endif
    keys.push_back(it.first);
    counts.push_back(it.second->count);
  }

#ifdef DEBUG
  std::cout << keys.size() << counts.size() << std::endl;
  print_text(keys);
  puts("before generating distribution");
#endif
  std::discrete_distribution<std::size_t> distribution(counts.begin(),
                                                       counts.end());

  auto idx = distribution(this->generator);
#ifdef DEBUG
  std::cout << idx << " " << keys.size() << std::endl;
#endif
  auto res = keys[idx];
#ifdef DEBUG
  std::cout << res << std::endl;
#endif
  return res;
}

std::string Parser::generate_text(const std::size_t length)
{
  // 1) Generate first word
  // 2) Generate words while n < depth
  // 3) Generate the rest of the text

  // 1)
  std::vector<std::string> text;
  const std::size_t depth = this->depth - 1;

#ifdef DEBUG
  puts("pushing first value");
#endif
  auto word = this->get_randomic_first_word();
  if (rank == MASTER) {

  }
  text.push_back();
#ifdef DEBUG
  puts("first word:");
  print_text(text);
#endif

  // 2)
#ifdef DEBUG
  puts("pushing second wave");
#endif
  for (std::size_t i=1; i<depth; i++) {
    std::string word = this->get_randomic_word(text, i);
#ifdef DEBUG
    std::cout << word;
    print_text(text);
#endif
    text.push_back(word);
#ifdef DEBUG
    puts("pushed!");
#endif
  }

  // 3)
#ifdef DEBUG
  puts("before limitless!!");
#endif
  for (auto i=depth; i<length; i++) {
    auto begin = tokens.begin() + i;
    auto end   = tokens.begin() + i + depth;
    std::vector<std::string> tmp_vec(begin, end);
    text.push_back(this->get_randomic_word(tmp_vec, depth));
#ifdef DEBUG
    print_text(text);
#endif
  }

  // join words into string
  std::stringstream ss;
  for (std::string word : text) {
    ss << word << " ";
  }
  return ss.str();
}
