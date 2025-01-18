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

  std::vector<std::string> getVariableNames() override {
    std::size_t body_nodes_count = 0;
    for (auto& thenBody_node : this->thenBody_) {
      assert(thenBody_node.get() != nullptr);
      body_nodes_count += thenBody_node->getVariableNames().size();
    }
    for (auto& elseBody_node : this->elseBody_) {
      assert(elseBody_node.get() != nullptr);
      body_nodes_count += elseBody_node->getVariableNames().size();
    }
    auto condition_var_names = condition_->getVariableNames();
    std::vector<std::string> values(condition_var_names.size() + body_nodes_count);

    size_t i = 0;
    for (auto& var : condition_var_names) {
      values[i] = var;
      i++;
    }

    for (auto& thenBody_node : this->thenBody_) {
      for (std::string name : thenBody_node->getVariableNames()) {
        values[i] = name;
        i++;
      }
    }

    for (auto& elseBody_node : this->elseBody_) {
      assert(elseBody_node.get() != nullptr);
      for (std::string name : elseBody_node->getVariableNames()) {
        values[i] = name;
        i++;
      }
    }
    return values;
  }

 private:
  ASTNode* condition_;
  std::vector<std::unique_ptr<ASTNode>> thenBody_;
  std::vector<std::unique_ptr<ASTNode>> elseBody_;
};

#endif // IF_NODE_H