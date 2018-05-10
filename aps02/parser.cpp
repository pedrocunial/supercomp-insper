#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>

#include "parser.hpp"

#define MASTER 0

namespace mpi = boost::mpi;

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
  this->tokenizer = new Tokenizer();

  this->rank = this->world.rank();
  this->size = this->world.size();

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
  // for (auto token : this->tokens) {
  //   std::cout << token << std::endl;
  // }
#endif
  const std::size_t rand_max = this->tokens.size() - this->depth;
  return this->tokens[std::rand() % rand_max];
}

Parser::word_return_t * Parser::get_randomic_word(std::vector<std::string> &key,
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
  std::size_t total = 0;
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
    total += it.second->count;
  }

#ifdef DEBUG
  std::cout << keys.size() << counts.size() << std::endl;
  print_text(keys);
  puts("before generating distribution");
#endif
  std::discrete_distribution<int> distribution(counts.begin(),
                                               counts.end());

  auto idx = distribution(this->generator);
#ifdef DEBUG
  std::cout << idx << " " << keys.size() << std::endl;
#endif
  auto res = keys[idx];
#ifdef DEBUG
  std::cout << res << std::endl;
#endif
  return new word_return_t(((double) counts[idx]) / ((double) total), res);
}

void Parser::send_word(const std::string &word, const double proba)
{
  mpi::request reqs[2];
  reqs[0] = this->world.isend(MASTER, SELECTED_WORD_TAG, std::string(word));
  reqs[1] = this->world.isend(MASTER, SELECTED_PERCENT_TAG, proba);
#ifdef DEBUG
  std::cout << "#send_word -- sending " << word << std::endl;
#endif
  mpi::wait_all(reqs, reqs + 2);
#ifdef DEBUG
  std::cout << "#send_word -- sent!! " << word << std::endl;
#endif
}

std::string Parser::select_word(const std::vector<std::string> &words,
                                const std::vector<double> &percents)
{
  std::discrete_distribution<int> distribution(percents.begin(),
                                               percents.end());
  return words[distribution(this->generator)];
}

std::string Parser::gather_words(const std::string &word, const double percent)
{
  std::size_t size = this->size - 1;
  mpi::request reqs[2 * size];
  std::vector<std::string> words(size + 1);
  std::vector<double> percents(size + 1);

  words[0] = word;
  percents[0] = percent;
  for (std::size_t i=0; i<size; i++) {
    reqs[i] = this->world.irecv(i+1, SELECTED_WORD_TAG, words[i]);
    reqs[i + size] = this->world.irecv(i+1, SELECTED_PERCENT_TAG, percents[i]);
  }
#ifdef DEBUG
  puts("#gather_words -- Before wait_all");
#endif
  mpi::wait_all(reqs, reqs + 2 * size);
#ifdef DEBUG
  puts("#gather_words -- after wait_all!!");
#endif
  return this->select_word(words, percents);
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
  std::string final_word;
  if (rank == MASTER) {
    final_word = this->gather_words(word, (double) 1.0);
  } else {
    this->send_word(word, (double) 1.0);
  }
  broadcast(this->world, final_word, MASTER);
  text.push_back(final_word);
#ifdef DEBUG
  puts("first word:");
  print_text(text);
#endif

  // 2)
#ifdef DEBUG
  puts("pushing second wave");
#endif
  for (std::size_t i=1; i<depth; i++) {
    std::string final_word;
    word_return_t *value = this->get_randomic_word(text, i);
    if (rank == MASTER)
      final_word = this->gather_words(value->word, value->proba);
    else
      this->send_word(value->word, value->proba);
    broadcast(this->world, final_word, MASTER);
    delete value;
#ifdef DEBUG
    std::cout << word;
#endif
    text.push_back(final_word);
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
    std::string final_word;
    word_return_t *value = this->get_randomic_word(tmp_vec, depth);
    if (rank == MASTER)
      final_word = this->gather_words(value->word, value->proba);
    else
      this->send_word(value->word, value->proba);
    broadcast(this->world, final_word, MASTER);
    delete value;
    text.push_back(final_word);
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
