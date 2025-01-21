#include <fstream>
#include <sstream>
#include <string>
#include "llvm-backend/JITExecutor.h"
#include "llvm-backend/IRGeneratorV2.h"
#include "parser/Parser.h"
#include "ASTToBytecodeConverter.h"
#include "VirtualMachine.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <source file>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file) {
    std::cerr << "Error: File " << argv[1] << " not found!" << std::endl;
    return 1;
  }

  std::stringstream input_buffer;
  input_buffer << file.rdbuf();
  std::string code = input_buffer.str();
  file.close();

  Parser parser(code);
  auto ast = parser.parse();

  auto bytecode = ASTToBytecodeConverter::generateBytecode(ast, argv[1]);

  VirtualMachine vm;
  vm.execute(bytecode);

  return 0;
}