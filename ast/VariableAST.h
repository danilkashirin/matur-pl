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

  std::vector<std::string> getVariableNames() override {
    return std::vector<std::string>();
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

  std::vector<std::string> getVariableNames() override {
    return value->getVariableNames();
  }

 private:
  std::string type;
  std::unique_ptr<ASTNode> value;
};

class VariableAssignAST : public VariableAST {
 public:
  VariableAssignAST(std::string name, std::unique_ptr<ASTNode> value)
      : VariableAST(std::move(name)), value(std::move(value)) {}

  [[nodiscard]] const ASTNode* getValue() const { return value.get(); }

  std::vector<std::string> getVariableNames() override {
    return value->getVariableNames();
  }

 private:
  std::unique_ptr<ASTNode> value;
};

class VariableRefAST : public ASTNode {
 public:
  explicit VariableRefAST(std::string name)
      : name(std::move(name)) {}

  [[nodiscard]] const std::string& getName() const { return name; }

  std::vector<std::string> getVariableNames() override {
    return std::vector<std::string>();
  }

 private:
  std::string name;
};

#endif // VARIABLE_AST_H