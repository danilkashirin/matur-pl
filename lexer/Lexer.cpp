#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <utility>

Lexer::Lexer(std::string  input) : input(std::move(input)), position(0) {}

Token Lexer::getNextToken() {
  skipWhitespace();
  if (position >= input.size()) {
    return {TokenType::EndOfFile, ""};
  }

  char current = input[position];

  if (std::isalpha(current)) {
    return parseIdentifierOrKeyword();
  }

  if (std::isdigit(current)) {
    return parseNumber();
  }

  switch (current) {
    case '=':
      position++;
      return {TokenType::Equals, "="};
    case ';':
      position++;
      return {TokenType::Semicolon, ";"};
    case '(':
      position++;
      return {TokenType::LParen, "("};
    case ')':
      position++;
      return {TokenType::RParen, ")"};
    default:
      throw std::runtime_error("Unexpected character: " + std::string(1, current));
  }
}

void Lexer::skipWhitespace() {
  while (position < input.size() && std::isspace(input[position])) {
    ++position;
  }
}

Token Lexer::parseIdentifierOrKeyword() {
  size_t start = position;
  while (position < input.size() && std::isalnum(input[position])) {
    ++position;
  }
  std::string value = input.substr(start, position - start);
  if (value == "print") {
    return {TokenType::Print, value};
  }
  if (value == "true" || value == "false") {
    return {TokenType::Boolean, value};
  }
  return {TokenType::Identifier, value};
}

Token Lexer::parseNumber() {
  size_t start = position;
  while (position < input.size() && std::isdigit(input[position])) {
    ++position;
  }
  return {TokenType::Number, input.substr(start, position - start)};
}