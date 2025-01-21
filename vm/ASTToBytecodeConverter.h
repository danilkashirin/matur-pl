#ifndef AST_TO_BYTECODE_CONVERTER_H
#define AST_TO_BYTECODE_CONVERTER_H

#include "ASTNode.h"
#include <vector>
#include <tuple>
#include <memory>
#include <string>

class ASTToBytecodeConverter {
 public:
  static std::vector<std::tuple<std::string, std::vector<int64_t>>>
  generateBytecode(const std::vector<std::unique_ptr<ASTNode>>& ast, char* src_filename);
};

#endif // AST_TO_BYTECODE_CONVERTER_H