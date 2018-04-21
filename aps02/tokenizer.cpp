#include <iostream>
#include <typeinfo>
#include <sstream>

#include "tokenizer.hpp"
#include "pugixml/pugixml.hpp"

Tokenizer::Tokenizer(void) {
}

Tokenizer::~Tokenizer(void) { }

void Tokenizer::load_file(const char *filename)
{
  pugi::xml_document doc;
  pugi::xml_parse_result loaded;

  loaded = doc.load_file(filename);
  if (!loaded) throw "Invalid file value";

  pugi::xml_node root = doc.child(ROOT_KEY);
  pugi::xml_node pages = root.child(PAGE);
  std::stringstream ss;

  for (pugi::xml_node node=pages; node;
       node=node.next_sibling(PAGE)) {
    // joins all contents into one big string
    ss << node.child_value(TITLE) << " ";
  }

  puts("donezo");
  std::string tmp = ss.str();
  // std::cout << tmp << std::endl;
  this->text = tmp;
  puts("hihihihi");
}
