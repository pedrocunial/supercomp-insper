#include "word-trie.hpp"

Node::Node(std::string word, std::size_t count)
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

void Node::add_child(std::string word, std::size_t count)
{
  if (this->children.count(word)) {  // check if map has key
    this->children[word]->count += count;
  } else {
    this->children[word] = new Node(word, count);
  }
}

WordTrie::WordTrie(std::string word)
{
  this->root = new Node(word, 0);
}

WordTrie::~WordTrie(void) {
  delete this->root;
}

