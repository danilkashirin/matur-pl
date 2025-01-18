#ifndef BOOLEAN_AST_H
#define BOOLEAN_AST_H

#include "ASTNode.h"

class BooleanAST : public ASTNode {
 public:
  explicit BooleanAST(bool value) : value(value) {}

  [[nodiscard]] bool getValue() const { return value; }

 private:
  bool value;
};

#endif // BOOLEAN_AST_H