#include "JITExecutor.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <iostream>

uint64_t executeIR(const std::string& irCode) {

  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  llvm::LLVMContext context;
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

  engine->finalizeObject();

  llvm::Function* mainFunc = engine->FindFunctionNamed("main");
  if (!mainFunc) {
    std::cerr << "Function 'main' not found\n";
    return 1;
  }

  std::vector<llvm::GenericValue> args;
  llvm::GenericValue result = engine->runFunction(mainFunc, args);

  delete engine;
  return result.IntVal.getSExtValue();
}