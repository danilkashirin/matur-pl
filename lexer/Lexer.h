#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>

class Lexer {
 public:
  Lexer(const std::string& input);

  Token getNextToken();

 private:
  void skipWhitespace();
  Token parseIdentifierOrKeyword();
  Token parseNumber();

  std::string input;
  size_t position;
};

#endif // LEXER_H