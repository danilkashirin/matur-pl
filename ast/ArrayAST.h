#ifndef ARRAY_ACCESS_AST_H
#define ARRAY_ACCESS_AST_H

#include "ASTNode.h"
#include <string>

class ArrayAccessAST : public ASTNode {
public:
    ArrayAccessAST(std::string  arrayName, int index)
            : arrayName(std::move(arrayName)), index(index) {}

    [[nodiscard]] const std::string& getArrayName() const { return arrayName; }
    [[nodiscard]] int getIndex() const { return index; }

private:
    std::string arrayName;
    int index;
};

#endif // ARRAY_ACCESS_AST_H