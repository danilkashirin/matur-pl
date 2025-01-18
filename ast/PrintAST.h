#ifndef PRINT_AST_H
#define PRINT_AST_H

#include "ASTNode.h"
#include <memory>
#include <cassert>

class PrintAST : public ASTNode {
 public:
  explicit PrintAST(std::unique_ptr<ASTNode> expression)
      : expression(std::move(expression)) {}

  [[nodiscard]] const ASTNode* getExpression() const { return expression.get(); }

  std::vector<std::string> getVariableNames() override {
    return expression->getVariableNames();
  }

 private:
  std::unique_ptr<ASTNode> expression;
};

#endif // PRINT_AST_H