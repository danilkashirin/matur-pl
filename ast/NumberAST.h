#ifndef NUMBER_AST_H
#define NUMBER_AST_H

#include "ASTNode.h"
#include <cassert>

class NumberAST : public ASTNode {
 public:
  explicit NumberAST(int64_t value) : value_(value) {}

  [[nodiscard]] int64_t getValue() const { return value_; }

  std::vector<std::string> getVariableNames() override {
    return std::vector<std::string>();
  }

 private:
  int64_t value_;
};

#endif // NUMBER_AST_H