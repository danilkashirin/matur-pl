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


  [[nodiscard]] std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

    auto indexBytecode = index->generateBytecode(currentOffset);
    currentOffset += indexBytecode.size();
    bytecode.insert(bytecode.end(), indexBytecode.begin(), indexBytecode.end());

    std::vector<int64_t> operands;
    operands.push_back(arrayName.size());
    for (char c : arrayName) {
      operands.push_back(static_cast<int64_t>(c));
    }

    bytecode.emplace_back("LOAD_ARRAY_ELEMENT", operands);

    return bytecode;
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

  [[nodiscard]] std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t currentOffset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;


    std::vector<int64_t> operands;
    operands.push_back(size);
    operands.push_back(name.size());
    for (char c : name) {
      operands.push_back(static_cast<int64_t>(c));
    }

    for (int64_t i = 0; i < size && i < static_cast<int64_t>(elements.size()); ++i) {
      operands.push_back(elements[i]);
    }

    for (int64_t i = elements.size(); i < size; ++i) {
      operands.push_back(0);
    }

    bytecode.emplace_back("DECLARE_ARRAY", operands);

    return bytecode;
  }


 private:
  std::string elementType;
  std::string name;
  int64_t size;
  std::vector<int64_t> elements;
};

#endif // ARRAY_ACCESS_AST_H