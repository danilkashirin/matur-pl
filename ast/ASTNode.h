#ifndef AST_NODE_H
#define AST_NODE_H

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <tuple>


class ASTNode {
 public:
  virtual ~ASTNode() = default;

  virtual std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const {
    return std::vector<std::tuple<std::string, std::vector<int64_t>>>();
  }
};

#endif // AST_NODE_H