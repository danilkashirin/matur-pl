#ifndef VARIABLE_AST_H
#define VARIABLE_AST_H

#include "ASTNode.h"
#include <string>
#include <memory>
#include <cassert>

class VariableAST : public ASTNode {
 public:
  explicit VariableAST(std::string name)
      : name(std::move(name)) {}

  [[nodiscard]] const std::string& getName() const { return name; }

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    return std::vector<std::tuple<std::string, std::vector<int64_t>>>();
  }

 private:
  std::string name;
};

class VariableDeclAST : public VariableAST {
 public:
  VariableDeclAST(std::string type, std::string name, std::unique_ptr<ASTNode> value)
      : VariableAST(std::move(name)), type(std::move(type)), value(std::move(value)) {}

  [[nodiscard]] const std::string& getType() const { return type; }
  [[nodiscard]] const ASTNode* getValue() const { return value.get(); }

  [[nodiscard]] std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    auto valueBytecode = value->generateBytecode(currentOffset);
    currentOffset += valueBytecode.size();
    bytecode.insert(bytecode.end(), valueBytecode.begin(), valueBytecode.end());

    std::string operation = "DECLARE_VAR";
    std::vector<int64_t> operands;
    operands.push_back(static_cast<int64_t>(getName().size()));
    for (char c : getName()) {
      operands.push_back(static_cast<int64_t>(c));
    }

    bytecode.emplace_back(operation, operands);

    return bytecode;
  }

 private:
  std::string type;
  std::unique_ptr<ASTNode> value;
};

class VariableRefAST : public ASTNode {
 public:
  explicit VariableRefAST(std::string name)
      : name(std::move(name)) {}

  [[nodiscard]] const std::string& getName() const { return name; }

  [[nodiscard]] std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<int64_t> nameOperands;
    nameOperands.push_back(name.size());
    for (char c : name) {
      nameOperands.push_back(static_cast<int64_t>(c));
    }
    return {{"LOAD_VAR", nameOperands}};
  }

 private:
  std::string name;
};

#endif // VARIABLE_AST_H