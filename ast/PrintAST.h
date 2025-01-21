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

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    auto expressionBytecode = expression->generateBytecode(currentOffset);
    currentOffset += expressionBytecode.size();
    expressionBytecode.emplace_back("PRINT", std::vector<int64_t>());
    return expressionBytecode;
  }


 private:
  std::unique_ptr<ASTNode> expression;
};

#endif // PRINT_AST_H