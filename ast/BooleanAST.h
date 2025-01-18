#ifndef BOOLEAN_AST_H
#define BOOLEAN_AST_H

#include "ASTNode.h"
#include <cassert>

class BooleanAST : public ASTNode {
 public:
  explicit BooleanAST(bool value) : value(value) {}

  [[nodiscard]] bool getValue() const { return value; }

  std::vector<std::string> getVariableNames() override {
    return std::vector<std::string>();
  }

 private:
  bool value;
};

#endif // BOOLEAN_AST_H