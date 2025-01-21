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

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    if (auto* arrayAccess = dynamic_cast<ArrayAccessAST*>(lhs.get())) {

      auto indexBytecode = arrayAccess->getIndex()->generateBytecode(currentOffset);
      currentOffset += indexBytecode.size();
      bytecode.insert(bytecode.end(), indexBytecode.begin(), indexBytecode.end());

      auto rhsBytecode = rhs->generateBytecode(currentOffset);
      currentOffset += rhsBytecode.size();
      bytecode.insert(bytecode.end(), rhsBytecode.begin(), rhsBytecode.end());

      std::vector<int64_t> operands;
      operands.push_back(arrayAccess->getArrayName().size());
      for (char c : arrayAccess->getArrayName()) {
        operands.push_back(static_cast<int64_t>(c));
      }
      bytecode.emplace_back("ASSIGN_ARRAY_ELEMENT", operands);
    } else if (auto* variable = dynamic_cast<VariableRefAST*>(lhs.get())) {

      auto rhsBytecode = rhs->generateBytecode(currentOffset);
      currentOffset += rhsBytecode.size();
      bytecode.insert(bytecode.end(), rhsBytecode.begin(), rhsBytecode.end());

      std::vector<int64_t> operands;
      operands.push_back(variable->getName().size());
      for (char c : variable->getName()) {
        operands.push_back(static_cast<int64_t>(c));
      }
      bytecode.emplace_back("ASSIGN_VAR", operands);
    } else {
      std::cerr << "Unsupported LHS type in AssignmentAST\n";
    }

    return bytecode;
  }

 private:
  std::unique_ptr<ASTNode> lhs;
  std::unique_ptr<ASTNode> rhs;
};

#endif // ASSIGNMENT_AST_H