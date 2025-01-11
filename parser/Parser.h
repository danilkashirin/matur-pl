#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "../lexer/Lexer.h"
#include "../ast/ASTNode.h"

class Parser {
 public:
  Parser(const std::string& input);

  std::vector<std::unique_ptr<ASTNode>> parse();

 private:
  void consumeToken();
  std::unique_ptr<ASTNode> parseVariableDeclaration();
  int64_t parseValue(const std::string& type);
  void expect(TokenType expectedType);
  std::unique_ptr<ASTNode> parsePrintStatement();

  Lexer lexer;
  Token currentToken;
};

#endif // PARSER_H