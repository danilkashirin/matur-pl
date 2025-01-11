#ifndef VARIABLE_DECL_AST_H
#define VARIABLE_DECL_AST_H

#include "ASTNode.h"
#include <string>

class VariableDeclAST : public ASTNode {
 public:
  VariableDeclAST(const std::string& type,const std::string& name, int64_t value)
      : type(type), name(name), value(value) {}

  const std::string& getType() const { return type; }
  const std::string& getName() const { return name; }
  int64_t getValue() const { return value; }

 private:
  std::string type;
  std::string name;
  int64_t value;
};

#endif // VARIABLE_DECL_AST_H