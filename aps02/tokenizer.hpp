#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <string>
#include "pugixml/pugixml.hpp"

#define ROOT_KEY "mediawiki"
#define TITLE "title"
#define PAGE "page"

class Tokenizer
{
public:
  std::string text;

  void load_file(const char *filename);
  Tokenizer(void);
  ~Tokenizer(void);

private:
};

#endif
