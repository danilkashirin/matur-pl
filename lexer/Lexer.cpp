#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <utility>

Lexer::Lexer(std::string input) : input(std::move(input)), position(0) {}

Token Lexer::getNextToken() {
  skipWhitespace();
  if (position >= input.size()) {
    return {TokenType::EndOfFile, ""};
  }

  char current = input[position];

  if (std::isalpha(current) || current == '_') {
    return parseIdentifierOrKeyword();
  }

  if (std::isdigit(current) || (current == '-' && (position + 1 < input.size() && std::isdigit(input[position + 1])))) {
    return parseNumber();
  }

  switch (current) {
    case '=':
      if (position + 1 < input.size() && input[position + 1] == '=') {
        position += 2;
        return {TokenType::EqualsEquals, "=="};
      }
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
    case '+':
      position++;
      return {TokenType::Plus, "+"};
    case '-':
      position++;
      return {TokenType::Minus, "-"};
    case '*':
      position++;
      return {TokenType::Asterisk, "*"};
    case '/':
      position++;
      return {TokenType::Slash, "/"};
    case '%':
      position++;
      return {TokenType::Modulo, "%"};
    case '<':
      position++;
      return {TokenType::LessThan, "<"};
    case '>':
      position++;
      return {TokenType::GreaterThan, ">"};
    case '[':
      position++;
      return {TokenType::LBracket, "["};
    case ']':
      position++;
      return {TokenType::RBracket, "]"};
    case ',':
      position++;
      return {TokenType::Comma, ","};
    case '{':
      position++;
      return {TokenType::CurlyLBracket, "{"};
    case '}':
      position++;
      return {TokenType::CurlyRBracket, "}"};
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
  while (position < input.size() && (std::isalnum(input[position]) || input[position] == '_')) {
    ++position;
  }

  std::string value = input.substr(start, position - start);

  if (value == "def") {
    return {TokenType::Def, value};
  }

  if (value == "return") {
    return {TokenType::Return, value};
  }

  if (value == "print") {
    return {TokenType::Print, value};
  }

  if (value == "true" || value == "false") {
    return {TokenType::Boolean, value};
  }

  if (value == "jawohl") {
    return {TokenType::Jawohl, value};
  }

  if (value == "int" || value == "bool") {
    return {TokenType::Type, value};
  }

  if (value == "array") {
    return {TokenType::Array, value};
  }

  if (value == "for") {
    return {TokenType::For, value};
  }

  if (value == "in") {
    return {TokenType::In, value};
  }

  if (value == "if") {
    return {TokenType::If, value};
  }

  if (value == "else") {
    return {TokenType::Else, value};
  }

  if (value == "random") {
    return {TokenType::Random, value};
  }

  return {TokenType::Identifier, value};
}

Token Lexer::parseNumber() {
  size_t start = position;

  if (input[position] == '-') {
    ++position;
  }

  while (position < input.size() && std::isdigit(input[position])) {
    ++position;
  }

  return {TokenType::Number, input.substr(start, position - start)};
}