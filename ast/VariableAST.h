#ifndef VARIABLE_DECL_AST_H
#define VARIABLE_DECL_AST_H

#include "ASTNode.h"
#include <string>
#include <memory>
#include <utility>

class VariableDeclAST : public ASTNode {
 public:
  VariableDeclAST(std::string type, std::string  name, std::unique_ptr<ASTNode> value)
      : type(std::move(type)), name(std::move(name)), value(std::move(value)) {}

  [[nodiscard]] const std::string& getType() const { return type; }
  [[nodiscard]] const std::string& getName() const { return name; }
  [[nodiscard]] const ASTNode* getValue() const { return value.get(); }

 private:
  std::string type;
  std::string name;
  std::unique_ptr<ASTNode> value;
};

#endif // VARIABLE_DECL_AST_H