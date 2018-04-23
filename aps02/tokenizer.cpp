#include <iostream>
#include <sstream>

#include "tokenizer.hpp"
#include "pugixml/pugixml.hpp"

Tokenizer::Tokenizer(void) {
  this->text = "";
}

Tokenizer::~Tokenizer(void) { }

void Tokenizer::load_file(const char *filename)
{
  pugi::xml_document doc;
  pugi::xml_parse_result loaded;

  loaded = doc.load_file(filename);
  if (!loaded) throw "Invalid file value";

  this->text = "";  // re-init text (allows memoization at get_tokens)

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

std::vector<std::string> Tokenizer::get_tokens(void)
{
  if (!this->tokens.empty()) return this->tokens;
  if (this->text.size() <= 0) throw "No text to be tokenized";

  std::istringstream iss(this->text);
  copy(std::istream_iterator<std::string>(iss),
       std::istream_iterator<std::string>(),
       back_inserter(this->tokens));
  return this->tokens;
}
