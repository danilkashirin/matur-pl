#ifndef PRINT_AST_H
#define PRINT_AST_H

#include "ASTNode.h"
#include <string>

class PrintAST : public ASTNode {
 public:
  PrintAST(const std::string& identifier) : identifier(identifier) {}

  const std::string& getIdentifier() const { return identifier; }

 private:
  std::string identifier;
};

#endif // PRINT_AST_H