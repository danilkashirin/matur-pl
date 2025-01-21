#ifndef BOOLEAN_AST_H
#define BOOLEAN_AST_H

#include "ASTNode.h"
#include <cassert>

class BooleanAST : public ASTNode {
 public:
  explicit BooleanAST(bool value) : value(value) {}

  [[nodiscard]] bool getValue() const { return value; }

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    std::string operation = "LOAD_CONST";
    std::vector<int64_t> operands = {getValue()};

    bytecode.emplace_back(operation, operands);

    return bytecode;
  }

 private:
  bool value;
};

#endif // BOOLEAN_AST_H