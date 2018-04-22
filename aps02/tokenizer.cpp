#include <iostream>
#include <typeinfo>
#include <sstream>

#include "tokenizer.hpp"
#include "pugixml/pugixml.hpp"

Tokenizer::Tokenizer(std::size_t n) {
  this->text = "";
  this->n = n;
}

Tokenizer::~Tokenizer(void) { }

void Tokenizer::load_file(const char *filename)
{
  pugi::xml_document doc;
  pugi::xml_parse_result loaded;

  loaded = doc.load_file(filename);
  if (!loaded) throw "Invalid file value";

  pugi::xml_node pages = doc.child(ROOT_KEY).child(PAGE);
  std::stringstream ss;

  for (pugi::xml_node node=pages; node != NULL; node=node.next_sibling(PAGE)) {
    // joins all contents into one big string
    // TODO: Clean string contents (keep only alpha numerical + ponctuation
    // values)
    ss << node.child(REVISION).child_value(TEXT) << " ";
  }
  this->text = ss.str();
}

void Tokenizer::generate_tokens(void)
{
}
