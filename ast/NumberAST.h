#ifndef NUMBER_AST_H
#define NUMBER_AST_H

#include "ASTNode.h"
#include <cassert>

#include "ASTNode.h"
#include <vector>
#include <string>
#include <tuple>


class NumberAST : public ASTNode {
 public:
  explicit NumberAST(int64_t value) : value_(value) {}

  [[nodiscard]] int64_t getValue() const { return value_; }

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    std::string operation = "LOAD_CONST";
    std::vector<int64_t> operands = {getValue()};

    bytecode.emplace_back(operation, operands);

    return bytecode;
  }

 private:
  int64_t value_;
};

#endif // NUMBER_AST_H