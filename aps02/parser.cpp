#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iostream>

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
    trie->root->increment();
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

  for (auto i=0; i<limit; i++) {
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
  const std::size_t rand_max = this->tokens.size() - this->depth;
  return this->tokens[std::rand() % rand_max];
}

std::string Parser::get_randomic_word(std::vector<std::string> &key,
                                      std::size_t depth)
{
  // Parses to the "depth"th node, and threats it as the leaf
  // uses a discrete distribution to do the "weighted" part of it
  WordTrie *trie = this->map[key[0]];

  puts("got trie");

  Node *node  = NULL;
  if (depth == 1) {
    node = trie->root;
    puts("got root!");
  } else {
    auto begin = key.begin() + 1;
    auto end   = key.begin() + depth;
    std::vector<std::string> tmp_vec(begin, end);
    node = trie->get_node(tmp_vec, depth);
    puts("got node!");
  }

  // words + counts found in node children
  std::vector<std::string> keys;
  std::vector<std::size_t> counts;
  puts("lets gooooo");
  for (auto const &it : node->children) {
    keys.push_back(it.first);
    counts.push_back(it.second->count);
  }

  puts("lmao dude i went XD");
  std::discrete_distribution<std::size_t> distribution(counts.begin(),
                                                       counts.end());
  puts("but did i really ??");

  auto idx = distribution(this->generator);
  std::cout << idx << " " << keys.size() << std::endl;
  auto res = keys[idx];
  std::cout << res << std::endl;
  return res;
}

void print_text(std::vector<std::string> &text)
{
  for (auto word : text) {
    std::cout << " " << word;
  }
  std::cout << std::endl;
}

std::string Parser::generate_text(const std::size_t length)
{
  // 1) Generate first word
  // 2) Generate words while n < depth
  // 3) Generate the rest of the text

  // 1)
  std::vector<std::string> text;
  const std::size_t depth = this->depth - 1;

  puts("pushing first value");
  text.push_back(this->get_randomic_first_word());

  // 2)
  puts("pushing second wave");
  auto limit = (length > depth) ? length : depth;
  for (std::size_t i=1; i<depth; i++) {
    puts("pushing...");
    std::string word = this->get_randomic_word(text, i);
    puts("generated word!!");
    std::cout << word;
    print_text(text);
    text.push_back(word);
    puts("pushed!");
  }

  // 3)
  puts("before limitless!!");
  for (auto i=depth; i<length; i++) {
    auto begin = tokens.begin() + i;
    auto end   = tokens.begin() + i + depth;
    std::vector<std::string> tmp_vec(begin, end);
    text.push_back(this->get_randomic_word(tmp_vec, depth));
  }

  // join words into string
  std::stringstream ss;
  for (std::string word : text) {
    ss << word << " ";
  }
  return ss.str();
}
