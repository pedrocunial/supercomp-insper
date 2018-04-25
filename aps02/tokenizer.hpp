#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <string>
#include <vector>
#include <locale>
#include "pugixml/pugixml.hpp"

#define REVISION "revision"
#define ROOT_KEY "mediawiki"
#define TITLE    "title"
#define TEXT     "text"
#define PAGE     "page"

inline bool is_punct(char c);

class Tokenizer
{
public:
  std::string text;

  std::vector<std::string> get_tokens(void);
  void load_file(const char *filename);
  Tokenizer(void);
  ~Tokenizer(void);

private:
  std::vector<std::string> tokens;

  const char *clean_str(const char *str);
};

#endif
