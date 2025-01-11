#ifndef VARIABLE_DECL_AST_H
#define VARIABLE_DECL_AST_H

#include "ASTNode.h"
#include <string>
#include <utility>

class VariableDeclAST : public ASTNode {
 public:
  VariableDeclAST(std::string  type,const std::string& name, int64_t value)
      : type(std::move(type)), name(name), value(value) {}

  [[nodiscard]] const std::string& getType() const { return type; }
  [[nodiscard]] const std::string& getName() const { return name; }
  [[nodiscard]] int64_t getValue() const { return value; }

 private:
  std::string type;
  std::string name;
  int64_t value;
};

#endif // VARIABLE_DECL_AST_H