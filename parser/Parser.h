#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <string>

#include "ArrayAST.h"
#include "ASTNode.h"
#include "ForNode.h"
#include "Lexer.h"

class Parser {
 public:
  explicit Parser(const std::string& input);

  std::vector<std::unique_ptr<ASTNode>> parse();

 private:
  Lexer lexer;
  Token currentToken;

  void consumeToken();

  void expect(TokenType expectedType);

  ASTNode* parseVariableDeclaration();
  ASTNode* parseAssignment();
  ASTNode* parsePrintStatement();

  ASTNode* parseExpression();
  ASTNode* parseTerm();
  ASTNode* parseFactor();

  ASTNode* parseArrayDeclaration();

  std::vector<int64_t> parseArrayElements(const std::string& elementType);

  int64_t parseSize();

  ASTNode* parseArrayAccess(const std::string& arrayName);

  int64_t parseValue(const std::string& type);

  ASTNode* parseAccessValueForArray();

  ASTNode* parseFor();

  ASTNode* parseIf();

  ASTNode* parseFunction();

  std::vector<std::string> parsesFunctionArguments();

  ASTNode* parseReturn();

  ASTNode* parseCallFunction(std::string name);
};

#endif // PARSER_H