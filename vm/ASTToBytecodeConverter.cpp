#include "ASTToBytecodeConverter.h"
#include <fstream>

std::vector<std::tuple<std::string, std::vector<int64_t>>>
ASTToBytecodeConverter::generateBytecode(const std::vector<std::unique_ptr<ASTNode>>& ast, char* src_filename) {
  std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;

  size_t currentOffset = 0;

  for (const auto& node : ast) {
    auto nodeBytecode = node->generateBytecode(currentOffset);
    currentOffset += nodeBytecode.size();
    bytecode.insert(bytecode.end(), nodeBytecode.begin(), nodeBytecode.end());
  }

  size_t filenameLength = std::strlen(src_filename);

  size_t newFilenameLength = filenameLength + 1;
  char* outputFilename = new char[newFilenameLength];
  std::strncpy(outputFilename, src_filename, filenameLength - 4);
  std::strcpy(outputFilename + filenameLength - 4, ".bytempl");

  std::ofstream bytecode_file(outputFilename);

  for (const auto& [instruction, operands] : bytecode) {
    bytecode_file << instruction << " ";
    for (const auto& operand : operands) {
      bytecode_file << operand << " ";
    }
    bytecode_file << std::endl;
  }

  delete[] outputFilename;
  return bytecode;
};