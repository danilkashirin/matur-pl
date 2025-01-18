#include <fstream>
#include <sstream>
#include <string>
#include "llvm-backend/JITExecutor.h"
#include "llvm-backend/IRGeneratorV2.h"
#include "parser/Parser.h"
#include "vm/LlvmParser.h"

int main(int argc, char* argv[]) {

  std::ifstream file(argv[1]);
  std::stringstream input_buffer;
  input_buffer << file.rdbuf();
  std::string code = input_buffer.str();
  file.close();

  Parser parser(code);
  auto ast = parser.parse();

  llvm::LLVMContext context;
  auto module = generateModuleIR(ast, context, (argc >= 3));

  executeIR(module);
}