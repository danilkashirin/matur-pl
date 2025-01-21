#ifndef IF_NODE_H
#define IF_NODE_H

#include <vector>
#include <memory>
#include <cassert>
#include "ASTNode.h"

class IfNode : public ASTNode {
 public:
  IfNode(ASTNode* condition,
         std::vector<std::unique_ptr<ASTNode>> thenBody,
         std::vector<std::unique_ptr<ASTNode>> elseBody)
      : condition_(condition), thenBody_(std::move(thenBody)), elseBody_(std::move(elseBody)) {}

  ~IfNode() override {
    delete condition_;
  }

  [[nodiscard]] ASTNode* getCondition() const { return condition_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getThenBody() const { return thenBody_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getElseBody() const { return elseBody_; }

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    auto conditionBytecode = condition_->generateBytecode(currentOffset);
    bytecode.insert(bytecode.end(), conditionBytecode.begin(), conditionBytecode.end());

    bytecode.emplace_back("JUMP_IF_FALSE", std::vector<int64_t>{0});
    size_t jumpIfFalseIndex = bytecode.size() - 1;

    auto then_offset = currentOffset + bytecode.size();
    for (const auto& stmt : thenBody_) {
      auto thenBytecode = stmt->generateBytecode(then_offset);
      then_offset += thenBytecode.size();
      bytecode.insert(bytecode.end(), thenBytecode.begin(), thenBytecode.end());
    }

    size_t jumpIndex = 0;
    if (!elseBody_.empty()) {
      bytecode.emplace_back("JUMP", std::vector<int64_t>{0});
      jumpIndex = bytecode.size() - 1;
    }

    std::get<1>(bytecode[jumpIfFalseIndex])[0] =
        elseBody_.empty() ? currentOffset + bytecode.size() : currentOffset + jumpIndex + 1;

    if (!elseBody_.empty()) {
      auto else_offset = currentOffset + bytecode.size();
      for (const auto& stmt : elseBody_) {
        auto elseBytecode = stmt->generateBytecode(else_offset);
        else_offset += elseBytecode.size();
        bytecode.insert(bytecode.end(), elseBytecode.begin(), elseBytecode.end());
      }

      std::get<1>(bytecode[jumpIndex])[0] = currentOffset + bytecode.size();
    }

    return bytecode;
  }
 private:
  ASTNode* condition_;
  std::vector<std::unique_ptr<ASTNode>> thenBody_;
  std::vector<std::unique_ptr<ASTNode>> elseBody_;
};

#endif // IF_NODE_H