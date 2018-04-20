#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include "pugixml/pugixml.hpp"

class Tokenizer
{
public:
  void load_file(std::string filename);
  Tokenizer(void);
  ~Tokenizer(void);

private:
  pugi::xml_document doc;
  pugi::xml_parse_result result;
};

#endif
