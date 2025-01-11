#ifndef IRGENERATOR_H
#define IRGENERATOR_H

#include <string>
#include <vector>
#include <memory>
#include "../ast/ASTNode.h"

std::string generateIR(const std::vector<std::unique_ptr<ASTNode>>& ast);

#endif // IRGENERATOR_H