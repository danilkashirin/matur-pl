#ifndef FOR_NODE_H
#define FOR_NODE_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <cassert>
#include "ASTNode.h"

class WhileAST : public ASTNode {
 public:
  WhileAST(ASTNode* condition,
          std::vector<std::unique_ptr<ASTNode>> body)
      : condition_(condition), body_(std::move(body)) {}

  ~WhileAST() override {
  }

  [[nodiscard]] ASTNode* getCondition() const { return condition_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getBody() const { return body_; }

  std::vector<std::string> getVariableNames() override {
    std::size_t body_nodes_count = 0;

    auto condition_var_names = condition_->getVariableNames();
    std::vector<std::string> values(condition_var_names.size() + body_nodes_count);

    size_t i = 0;
    for (auto& var : condition_var_names) {
      values[i] = var;
      i++;
    }

    return values;
  }


 private:
  ASTNode* condition_;
  std::vector<std::unique_ptr<ASTNode>> body_;
};

#endif // FOR_NODE_H