#ifndef __WORD_TRIE_HPP__
#define __WORD_TRIE_HPP__

#include <string>
#include <vector>
#include <map>

class Node
{
public:
  std::string word;
  std::size_t count;
  std::map<std::string, Node*> children;

  Node *get_child(std::string &word);
  void add_child(std::string &word, std::size_t count);
  Node(std::string &word, std::size_t count);
  ~Node(void);
};

class WordTrie
{
public:
  Node *root;

  WordTrie(std::string &word, std::size_t depth);
  ~WordTrie(void);
  void add_word(std::vector<std::string> words);

private:
  std::size_t depth;
};

#endif
