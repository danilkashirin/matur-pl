#ifndef NUMBER_NODE_H
#define NUMBER_NODE_H

#include "ASTNode.h"

class NumberNode : public ASTNode {
 public:
  explicit NumberNode(int64_t value) : value_(value) {}

  int64_t GetValue() const { return value_; }

 private:
  int64_t value_;
};

#endif // NUMBER_NODE_H