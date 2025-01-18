#ifndef ARRAY_ACCESS_AST_H
#define ARRAY_ACCESS_AST_H

#include "ASTNode.h"
#include <string>
#include <cassert>

class ArrayAccessAST : public ASTNode {
 public:
  ArrayAccessAST(std::string arrayName, ASTNode* index)
      : arrayName(std::move(arrayName)), index(index) {}

  [[nodiscard]] const std::string& getArrayName() const { return arrayName; }
  [[nodiscard]] ASTNode* getIndex() const { return index; }

  std::vector<std::string> getVariableNames() override {
    std::vector<std::string> values(1);
    values[0] = this->arrayName;
    return values;
  }

 private:
  std::string arrayName;
  ASTNode* index;
};

class ArrayDeclAST : public ASTNode {
 public:
  ArrayDeclAST(std::string elementType, std::string name, int64_t size, const std::vector<int64_t>& elements)
      : elementType(std::move(elementType)), name(std::move(name)), size(size), elements(elements) {}

  [[nodiscard]] const std::string& getElementType() const { return elementType; }
  [[nodiscard]] const std::string& getName() const { return name; }
  [[nodiscard]] int64_t getSize() const { return size; }
  [[nodiscard]] const std::vector<int64_t>& getElements() const { return elements; }

  std::vector<std::string> getVariableNames() override {
    std::vector<std::string> values(1);
    values[0] = this->name;
    return values;
  }

 private:
  std::string elementType;
  std::string name;
  int64_t size;
  std::vector<int64_t> elements;
};

#endif // ARRAY_ACCESS_AST_H