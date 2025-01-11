#include "Parser.h"
#include "VariableDeclAST.h"
#include "PrintAST.h"
#include <stdexcept>

Parser::Parser(const std::string& input)
    : lexer(input), currentToken(lexer.getNextToken()) {}

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
  std::vector<std::unique_ptr<ASTNode>> nodes;

  while (currentToken.type != TokenType::EndOfFile) {
    if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::Boolean) {
      nodes.push_back(parseVariableDeclaration());
    } else if (currentToken.type == TokenType::Print) {
      nodes.push_back(parsePrintStatement());
    } else {
      throw std::runtime_error("Unexpected token during parsing");
    }
  }

  return nodes;
}

void Parser::consumeToken() {
  currentToken = lexer.getNextToken();
}

std::unique_ptr<ASTNode> Parser::parseVariableDeclaration() {
  std::string type = currentToken.value;
  consumeToken();
  std::string name = currentToken.value;
  consumeToken();

  expect(TokenType::Equals);
  int64_t value = parseValue(type);
  expect(TokenType::Semicolon);

  return std::make_unique<VariableDeclAST>(type, name, value);
}

int64_t Parser::parseValue(const std::string& type) {
  if (type == "bool") {
    std::string value = currentToken.value;
    consumeToken();
    return value == "true" ? 1 : 0;
  } else if (currentToken.type == TokenType::Number) {
    int64_t numValue = std::stoll(currentToken.value);
    consumeToken();
    return numValue;
  } else {
    throw std::runtime_error("Unexpected value type");
  }
}

void Parser::expect(TokenType expectedType) {
  if (currentToken.type != expectedType) {
    throw std::runtime_error("Expected a different token type");
  }
  consumeToken();
}

std::unique_ptr<ASTNode> Parser::parsePrintStatement() {
  consumeToken();
  expect(TokenType::LParen);

  std::string identifier = currentToken.value;
  consumeToken();

  expect(TokenType::RParen);
  expect(TokenType::Semicolon);

  return std::make_unique<PrintAST>(identifier);
}