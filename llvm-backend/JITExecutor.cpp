#include "JITExecutor.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <iostream>

uint64_t executeIR(std::unique_ptr<llvm::Module>& module) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetDisassembler();

  std::string errStr;
  llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move(module))
      .setErrorStr(&errStr)
      .setEngineKind(llvm::EngineKind::JIT)
      .create();

  if (!engine) {
    std::cerr << "Failed to create ExecutionEngine: " << errStr << "\n";
    return 1;
  }

  try {
    engine->finalizeObject();
  } catch (const std::exception& e) {
    std::cerr << "Exception during finalizeObject: " << e.what() << "\n";
    delete engine;
    return 1;
  }

  llvm::Function* mainFunc = engine->FindFunctionNamed("main");
  if (!mainFunc) {
    std::cerr << "Function 'main' not found\n";
    delete engine;
    return 1;
  }

  std::vector<llvm::GenericValue> args;
  llvm::GenericValue result = engine->runFunction(mainFunc, args);

  delete engine;
  return result.IntVal.getSExtValue();
}