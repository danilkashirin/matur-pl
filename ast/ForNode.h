#ifndef FOR_NODE_H
#define FOR_NODE_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <cassert>
#include "ASTNode.h"

class ForNode : public ASTNode {
 public:
  ForNode(std::string iteratorName,
          ASTNode* start,
          ASTNode* finish,
          ASTNode* step,
          std::vector<std::unique_ptr<ASTNode>> body)
      : iteratorName_(std::move(iteratorName)), start_(start), finish_(finish), step_(step), body_(std::move(body)) {}

  ~ForNode() override {
    delete start_;
    delete finish_;
    delete step_;
  }

  [[nodiscard]] const std::string& getIteratorName() const { return iteratorName_; }
  [[nodiscard]] ASTNode* getStart() const { return start_; }
  [[nodiscard]] ASTNode* getFinish() const { return finish_; }
  [[nodiscard]] ASTNode* getStep() const { return step_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getBody() const { return body_; }

  [[nodiscard]] std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    auto startBytecode = getStart()->generateBytecode(currentOffset);
    bytecode.insert(bytecode.end(), startBytecode.begin(), startBytecode.end());

    std::vector<int64_t> operands;
    operands.push_back(static_cast<int64_t>(getIteratorName().size()));
    for (char c : getIteratorName()) {
      operands.push_back(static_cast<int64_t>(c));
    }

    bytecode.emplace_back("DECLARE_VAR", operands);

    size_t loopStartOffset = currentOffset + bytecode.size();

    bytecode.emplace_back("LOAD_VAR", operands);
    auto finishBytecode = getFinish()->generateBytecode(currentOffset + bytecode.size());
    bytecode.insert(bytecode.end(), finishBytecode.begin(), finishBytecode.end());
    bytecode.emplace_back("LESS_THAN", std::vector<int64_t>{});

    size_t jumpToEndIndex = bytecode.size();
    bytecode.emplace_back("JUMP_IF_FALSE", std::vector<int64_t>{0});

    size_t bodyOffset = currentOffset + bytecode.size();
    for (const auto& stmt : getBody()) {
      auto bodyBytecode = stmt->generateBytecode(bodyOffset);
      bodyOffset += bodyBytecode.size();
      bytecode.insert(bytecode.end(), bodyBytecode.begin(), bodyBytecode.end());
    }

    bytecode.emplace_back("LOAD_VAR", operands);
    if (getStep()) {
      auto stepBytecode = getStep()->generateBytecode(currentOffset + bytecode.size());
      bytecode.insert(bytecode.end(), stepBytecode.begin(), stepBytecode.end());
    } else {
      bytecode.emplace_back("LOAD_CONST", std::vector<int64_t>{1});
    }
    bytecode.emplace_back("ADD", std::vector<int64_t>{});
    bytecode.emplace_back("ASSIGN_VAR", operands);

    bytecode.emplace_back("JUMP", std::vector<int64_t>{static_cast<int64_t>(loopStartOffset)});

    size_t loopEndOffset = currentOffset + bytecode.size();
    std::get<1>(bytecode[jumpToEndIndex])[0] = static_cast<int64_t>(loopEndOffset);

    return bytecode;
  }
 private:
  std::string iteratorName_;
  ASTNode* start_;
  ASTNode* finish_;
  ASTNode* step_;
  std::vector<std::unique_ptr<ASTNode>> body_;
};

#endif // FOR_NODE_H