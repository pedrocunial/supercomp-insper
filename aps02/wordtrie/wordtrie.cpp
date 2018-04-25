#include "wordtrie.hpp"

#ifdef DEBUG
#include <iostream>
#endif

Node::Node(std::string &word, std::size_t count)
{
  this->word = word;
  this->count = count;
}

Node::~Node(void)
{
  std::map<std::string, Node*> c = this->children;
  for (std::map<std::string, Node*>::iterator it=c.begin(); it!=c.end(); ++it)
    delete it->second;
}

void Node::increment(void)
{
  this->count++;
}

void Node::decrement(void)
{
  // unsure if I'll use this
  this->count--;
}

void Node::add_child(std::string &word, std::size_t count)
{
  if (this->children.count(word)) {  // check if map has key
    this->children[word]->count += count;
  } else {
    this->children[word] = new Node(word, count);
  }
}

Node * Node::get_child(std::string &word)
{
#ifdef DEBUG
  if (!this->children.count(word))
    std::cout << this->word << " has no child" << word << std::endl;
#endif
  return this->children[word];
}

WordTrie::WordTrie(std::string &word, std::size_t depth)
{
  this->root = new Node(word, 1);
  this->depth = depth;
}

WordTrie::~WordTrie(void) {
  delete this->root;
}

void WordTrie::add_word(std::vector<std::string> words)
{
  // recieves a vector of words WITHOUT THE ROOT
  if (words.size() != this->depth - 1) {
    throw "Unexpected vector size (different from trie depth)";
  }

  Node *node = this->root;
  size_t i = 1, size = words.size();
  do {
    node->add_child(words[i], 1);
    node->increment();
    node = node->get_child(words[i]);
    i++;
  } while (i < size);
}

Node *WordTrie::get_node(std::vector<std::string> &words, std::size_t depth)
{
  // recieves a vector of words WITHOUT THE ROOT and the max depth to be
  // searched
#ifdef DEBUG
  std::cout << "depth: " << depth << std::endl;
#endif
  if (depth == 1) return this->root;

  Node *node = this->root;
  for (auto i=1; i<words.size(); i++) {
    node = node->get_child(words[i]);
#ifdef DEBUG
    puts("#get_node iteration");
#endif
  }
#ifdef DEBUG
  std::cout << "node is null? " << (node == nullptr) << std::endl;
  std::cout << "node value: " << node->word << std::endl;
#endif
  return node;
}

