#ifndef COMPARE_OP_NODE_H
#define COMPARE_OP_NODE_H

#include "ASTNode.h"
#include <cassert>

class CompareOpNode : public ASTNode {
 public:
  enum class Operator {
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN_OR_EQUAL,
    EQUALS
  };

  CompareOpNode(ASTNode* left, Operator op, ASTNode* right)
      : left_(left), op_(op), right_(right) {}

  ~CompareOpNode() override {
    delete left_;
    delete right_;
  }

  [[nodiscard]] ASTNode* getLeft() const { return left_; }
  [[nodiscard]] ASTNode* getRight() const { return right_; }
  [[nodiscard]] Operator getOperator() const { return op_; }

  std::vector<std::string> getVariableNames() override {
    assert(this->left_ != nullptr);
    assert(this->right_ != nullptr);
    std::vector<std::string> lhs_names = this->left_->getVariableNames();
    std::vector<std::string> rhs_names = this->right_->getVariableNames();
    std::vector<std::string> values(lhs_names.size() + rhs_names.size());
    for (std::size_t i = 0; i < lhs_names.size(); ++i) {
        values[i] = lhs_names[i];
    }
    for (std::size_t i = lhs_names.size(); i < lhs_names.size() + rhs_names.size(); ++i) {
        values[i] = rhs_names[i];
    }
    return values;
  }

 private:
  ASTNode* left_;
  Operator op_;
  ASTNode* right_;
};

#endif // COMPARE_OP_NODE_H