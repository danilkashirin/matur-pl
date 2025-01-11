#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "llvm-backend/JITExecutor.h"
#include "llvm-backend/IRGenerator.h"
#include "parser/Parser.h"


int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: <program> <input IR file>\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  std::stringstream input_buffer;
  input_buffer << file.rdbuf();
  std::string code = input_buffer.str();
  file.close();

  Parser parser(code);
  auto ast = parser.parse();
  std::string irCode = generateIR(ast);

  return static_cast<int>(executeIR(irCode));
}