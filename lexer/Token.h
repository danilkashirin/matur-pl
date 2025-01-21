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
  Plus,
  Minus,
  Asterisk,
  Slash,
  Type,
  EndOfFile,
  Array,
  LessThan,
  GreaterThan,
  LBracket,
  RBracket,
  Comma,
  Modulo,
  LessThanOrEqual,
  GreaterThanOrEqual,
  EqualsEquals,
  For,
  In,
  CurlyLBracket,
  CurlyRBracket,
  If,
  Else,
  Random,
  Def,
  Return,
  Jawohl,
};

struct Token {
  TokenType type;
  std::string value;
};

#endif // TOKEN_H