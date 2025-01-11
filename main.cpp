#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include "llvm.h"

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

  // Initialize necessary components to run LLVM IR
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::LLVMContext context;

  // Generate IR from AST
  Parser parser(code);
  auto ast = parser.parse();
  std::string irCode = generateIR(ast);

  // Load the generated IR code into an LLVM module
  llvm::SMDiagnostic err;
  std::unique_ptr<llvm::Module> module = llvm::parseIR(llvm::MemoryBufferRef(irCode, "module"), err, context);

  if (!module) {
    std::cerr << "Error reading IR: " << err.getMessage().str() << "\n";
    return 1;
  }

  std::string errStr;
  llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move(module))
      .setErrorStr(&errStr)
      .setEngineKind(llvm::EngineKind::JIT)
      .create();

  if (!engine) {
    std::cerr << "Failed to create ExecutionEngine: " << errStr << "\n";
    return 1;
  }

  // Optionally you can optimize the module if required
  engine->finalizeObject();

  // Assume there is a function named "main" in the LLVM IR
  llvm::Function* mainFunc = engine->FindFunctionNamed("main");

  if (!mainFunc) {
    std::cerr << "Function 'main' not found\n";
    return 1;
  }

  // Execute the "main" function
  std::vector<llvm::GenericValue> args;
  llvm::GenericValue result = engine->runFunction(mainFunc, args);

  // Assuming the main function returns an integer
  std::cout << "Result: " << result.IntVal.getSExtValue() << "\n";

  delete engine;
  return 0;
}