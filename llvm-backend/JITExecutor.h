#ifndef JIT_EXECUTOR_H
#define JIT_EXECUTOR_H

#include <string>
#include <llvm/IR/Module.h>

uint64_t executeIR(std::unique_ptr<llvm::Module>& module);

#endif // JIT_EXECUTOR_H