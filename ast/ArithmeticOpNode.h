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

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    auto leftBytecode = left_->generateBytecode(currentOffset);
    currentOffset += leftBytecode.size();
    auto rightBytecode = right_->generateBytecode(currentOffset);
    currentOffset += rightBytecode.size();

    bytecode.insert(bytecode.end(), leftBytecode.begin(), leftBytecode.end());
    bytecode.insert(bytecode.end(), rightBytecode.begin(), rightBytecode.end());

    switch (op_) {
      case Operator::ADD: bytecode.emplace_back("ADD", std::vector<int64_t>());
        break;
      case Operator::SUBTRACT: bytecode.emplace_back("SUBTRACT", std::vector<int64_t>());
        break;
      case Operator::MULTIPLY: bytecode.emplace_back("MULTIPLY", std::vector<int64_t>());
        break;
      case Operator::DIVIDE: bytecode.emplace_back("DIVIDE", std::vector<int64_t>());
        break;
      case Operator::MODULO: bytecode.emplace_back("MODULO", std::vector<int64_t>());
        break;
    }

    return bytecode;
  }

 private:
  ASTNode* left_;
  Operator op_;
  ASTNode* right_;
};

#endif // ARITHMETIC_OP_NODE_H