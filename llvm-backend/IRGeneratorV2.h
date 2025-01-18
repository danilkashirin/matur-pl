#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include <memory>
#include <map>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "PrintAST.h"
#include "VariableAST.h"
#include "NumberAST.h"
#include "ForNode.h"
#include "IRGeneratorV2.h"
#include "BooleanAST.h"
#include "ArrayAST.h"
#include "IfNode.h"
#include "ArithmeticOpNode.h"
#include "AssigmentAST.h"
#include "CompareOpNode.h"
#include <llvm/IRReader/IRReader.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar/GVN.h>

// Функции генерации IR для различных нод
llvm::Value* generateIRForNumber(const NumberAST* node, llvm::IRBuilder<>& builder);
llvm::Value* generateIRForBoolean(const BooleanAST* node, llvm::IRBuilder<>& builder);
llvm::Value* generateIRForVariableDecl(const VariableDeclAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForVariableAssign(const VariableAssignAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForVariableRef(const VariableRefAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForPrint(const PrintAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForIfNode(const IfNode* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForArithmeticOpNode(const ArithmeticOpNode* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForCompareOpNode(const CompareOpNode* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForArrayDecl(const ArrayDeclAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForArrayAccess(const ArrayAccessAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);
llvm::Value* generateIRForAssignment(const AssignmentAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);

// Главная функция для генерации IR
llvm::Value* generateIR(const ASTNode* node,
                        llvm::IRBuilder<>& builder,
                        llvm::Module& module,
                        llvm::Function* parentFunction);

llvm::Value* generateIRForForNode(const ForNode* forNode, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunctione);


// Генерация модуля IR из AST
std::unique_ptr<llvm::Module> generateModuleIR(std::vector<std::unique_ptr<ASTNode>>& astNodes, llvm::LLVMContext& context, bool withOpt);

#endif // IR_GENERATOR_H