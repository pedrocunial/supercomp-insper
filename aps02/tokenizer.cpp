#include "tokenizer.hpp"
#include "pugixml/pugixml.hpp"

Tokenizer::Tokenizer(void) { }
Tokenizer::~Tokenizer(void) { }

void Tokenizer::load_file(std::string filename)
{
  this->result = this->doc.load_file(filename.c_str());
}
