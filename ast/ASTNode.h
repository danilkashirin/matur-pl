#ifndef AST_NODE_H
#define AST_NODE_H

#include <iostream>
#include <cassert>

class ASTNode {
 public:
  virtual ~ASTNode() = default;
  virtual std::vector<std::string> getVariableNames() {
    return std::vector<std::string>();
  };
};

#endif // AST_NODE_H