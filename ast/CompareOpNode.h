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

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    auto leftBytecode = left_->generateBytecode(currentOffset);
    currentOffset += leftBytecode.size();
    auto rightBytecode = right_->generateBytecode(currentOffset);
    currentOffset += rightBytecode.size();

    bytecode.insert(bytecode.end(), leftBytecode.begin(), leftBytecode.end());
    bytecode.insert(bytecode.end(), rightBytecode.begin(), rightBytecode.end());

    switch (op_) {
      case Operator::LESS_THAN: bytecode.emplace_back("LESS_THAN", std::vector<int64_t>());
        break;
      case Operator::GREATER_THAN: bytecode.emplace_back("GREATER_THAN", std::vector<int64_t>());
        break;
      case Operator::LESS_THAN_OR_EQUAL: bytecode.emplace_back("LESS_THAN_OR_EQUAL", std::vector<int64_t>());
        break;
      case Operator::GREATER_THAN_OR_EQUAL: bytecode.emplace_back("GREATER_THAN_OR_EQUAL", std::vector<int64_t>());
        break;
      case Operator::EQUALS: bytecode.emplace_back("EQUALS", std::vector<int64_t>());
        break;
    }

    return bytecode;
  }

 private:
  ASTNode* left_;
  Operator op_;
  ASTNode* right_;
};

#endif // COMPARE_OP_NODE_H