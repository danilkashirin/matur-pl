#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
  Identifier,
  Number,
  Boolean,
  Print,
  Equals,
  Semicolon,
  LParen,
  RParen,
  EndOfFile
};

struct Token {
  TokenType type;
  std::string value;
};

#endif // TOKEN_H