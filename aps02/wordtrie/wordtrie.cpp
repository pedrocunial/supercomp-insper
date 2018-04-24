#include "wordtrie.hpp"

#include <iostream>

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
  if (!this->children.count(word))
    std::cout << this->word << " has no child" << word << std::endl;
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
  node->count++;
  for (std::string word : words) {
    node->add_child(word, 1);
    node = node->get_child(word);
  }
}

Node *WordTrie::get_node(std::vector<std::string> &words, std::size_t depth)
{
  // recieves a vector of words WITHOUT THE ROOT and the max depth to be
  // searched
  // if (words.size() > depth)
  //   throw "Unexpected vector size -- words vector > max depth";

  if (depth == 1) return this->root;

  Node *node = this->root;
  for (std::string word : words) {
    node = node->get_child(word);
    puts("ohaio");
  }
  return node;
}

