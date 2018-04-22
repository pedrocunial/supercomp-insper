#ifndef __WORD_TRIE_HPP__
#define __WORD_TRIE_HPP__

#include <string>
#include <map>

class Node
{
public:
  std::string word;
  std::size_t count;
  std::map<std::string, Node*> children;

  void add_child(std::string word, std::size_t count);
  Node(std::string word, std::size_t count);
  ~Node(void);
};

class WordTrie
{
public:
  Node *root;

  WordTrie(std::string word);
  ~WordTrie(void);

private:
};

#endif
