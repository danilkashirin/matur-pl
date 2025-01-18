#ifndef ASSIGNMENT_AST_H
#define ASSIGNMENT_AST_H

#include "ASTNode.h"
#include <memory>
#include <cassert>

class AssignmentAST : public ASTNode {
 public:
  AssignmentAST(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
      : lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    [[nodiscard]] ASTNode* getLHS() const { return lhs.get(); }
    [[nodiscard]] ASTNode* getRHS() const { return rhs.get(); }

  std::vector<std::string> getVariableNames() override {
    assert(this->lhs.get() != nullptr);
    assert(this->rhs.get() != nullptr);
    std::vector<std::string> lhs_names = this->lhs->getVariableNames();
    std::vector<std::string> rhs_names = this->rhs->getVariableNames();
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
  std::unique_ptr<ASTNode> lhs;

  std::unique_ptr<ASTNode> rhs;
};

#endif // ASSIGNMENT_AST_H