#include <iostream>
#include <map>
#include <sstream>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "parser.h"

std::string generateIR(const std::vector<std::unique_ptr<ASTNode>>& ast) {
  llvm::LLVMContext context;
  auto module = std::make_unique<llvm::Module>("my module", context);
  llvm::IRBuilder<> builder(context);

  llvm::FunctionType* funcType = llvm::FunctionType::get(builder.getInt32Ty(), false);
  llvm::Function
      * mainFunction = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module.get());
  llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", mainFunction);
  builder.SetInsertPoint(entry);

  std::map<std::string, llvm::Value*> namedValues;

  for (const auto& node : ast) {
    if (auto* varDecl = dynamic_cast<VariableDeclAST*>(node.get())) {
      llvm::Value* value;
      if (varDecl->getType() == "i32") {
        value = builder.getInt32(static_cast<int>(varDecl->getValue()));
      } else if (varDecl->getType() == "i64") {
        value = builder.getInt64(varDecl->getValue());
      } else if (varDecl->getType() == "bool") {
        value = builder.getInt1(varDecl->getValue());
      } else {
        throw std::runtime_error("Unsupported type for variable");
      }
      namedValues[varDecl->getName()] = value;
    } else if (auto* printAST = dynamic_cast<PrintAST*>(node.get())) {
      auto varIter = namedValues.find(printAST->getIdentifier());
      if (varIter == namedValues.end()) {
        std::cerr << "Variable to print not found!" << std::endl;
        return "";
      }

      llvm::Value* printValue = varIter->second;
      llvm::FunctionType* printfType = llvm::FunctionType::get(builder.getInt32Ty(), {builder.getInt8PtrTy()}, true);
      llvm::FunctionCallee printfFunc = module->getOrInsertFunction("printf", printfType);

      llvm::Value* formatStr;
      if (auto* intType = llvm::dyn_cast<llvm::IntegerType>(printValue->getType())) {
        if (intType->getBitWidth() == 1) {
          formatStr = builder.CreateGlobalStringPtr("%d\n");
        } else if (intType->getBitWidth() == 64) {
          formatStr = builder.CreateGlobalStringPtr("%lld\n");
        } else {
          formatStr = builder.CreateGlobalStringPtr("%d\n");
        }
      }
      builder.CreateCall(printfFunc, {formatStr, printValue});
    }
  }

  builder.CreateRet(builder.getInt32(0));

  std::string irString;
  llvm::raw_string_ostream irStream(irString);
  module->print(irStream, nullptr);
  return irStream.str();
}