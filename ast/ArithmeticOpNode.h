#ifndef ARITHMETIC_OP_NODE_H
#define ARITHMETIC_OP_NODE_H

#include <cassert>
#include "ASTNode.h"

class ArithmeticOpNode : public ASTNode {
 public:
  enum class Operator {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO
  };

  ArithmeticOpNode(ASTNode* left, Operator op, ASTNode* right)
      : left_(left), op_(op), right_(right) {}

  ~ArithmeticOpNode() override {
    delete left_;
    delete right_;
  }

  [[nodiscard]] ASTNode* getLeft() const { return left_; }
  [[nodiscard]] ASTNode* getRight() const { return right_; }
  [[nodiscard]] Operator getOperator() const { return op_; }

std::vector<std::string> getVariableNames() override {
    assert(this->left_ != nullptr);
    assert(this->right_ != nullptr);
    std::vector<std::string> left_names = this->left_->getVariableNames();
    std::vector<std::string> right_names = this->right_->getVariableNames();
    std::vector<std::string> values(left_names.size() + right_names.size());
    for (std::size_t i = 0; i < left_names.size(); ++i) {
        values[i] = left_names[i];
    }
    for (std::size_t i = left_names.size(); i < left_names.size() + right_names.size(); ++i) {
        values[i] = right_names[i];
    }
    return values;
  }

 private:
  ASTNode* left_;
  Operator op_;
  ASTNode* right_;
};

#endif // ARITHMETIC_OP_NODE_H