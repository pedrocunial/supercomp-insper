#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <string>
#include "pugixml/pugixml.hpp"

#define REVISION "revision"
#define ROOT_KEY "mediawiki"
#define TITLE    "title"
#define TEXT     "text"
#define PAGE     "page"

class Tokenizer
{
public:
  std::string text;

  void generate_tokens(void);
  void load_file(const char *filename);
  Tokenizer(std::size_t n);
  ~Tokenizer(void);

private:
  std::size_t n;
};

#endif
