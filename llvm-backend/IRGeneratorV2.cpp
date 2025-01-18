#include <memory>
#include <map>
#include <fstream>
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
#include <llvm/IRReader/IRReader.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

std::map<std::string, llvm::AllocaInst*> namedValues;

llvm::Value* generateIRForNumber(const NumberAST* node, llvm::IRBuilder<>& builder) {
  return llvm::ConstantInt::get(builder.getInt64Ty(), node->getValue());
}

llvm::Value* generateIRForBoolean(const BooleanAST* node, llvm::IRBuilder<>& builder) {
  return llvm::ConstantInt::get(builder.getInt1Ty(), node->getValue());
}

llvm::Value* generateIRForVariableDecl(const VariableDeclAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::Type* type;
  if (node->getType() == "int") {
    type = builder.getInt64Ty();
  } else {
    return nullptr;
  }

  llvm::Function* function = builder.GetInsertBlock()->getParent();
  llvm::IRBuilder<> tempBuilder(&function->getEntryBlock(), function->getEntryBlock().begin());

  llvm::AllocaInst* alloca = tempBuilder.CreateAlloca(type, nullptr, node->getName());
  if (alloca) {
    namedValues[node->getName()] = alloca;
  }

  llvm::Value* initialValue = generateIR(node->getValue(), builder, module, parentFunction);
  builder.CreateStore(initialValue, alloca);

  return alloca;
}

llvm::Value* generateIRForVariableAssign(const VariableAssignAST* node,
                                         llvm::IRBuilder<>& builder,
                                         llvm::Module& module,
                                         llvm::Function* parentFunction) {
  llvm::Value* value = generateIR(node->getValue(), builder, module, parentFunction);

  llvm::AllocaInst* variable = namedValues[node->getName()];
  if (!variable) {
    llvm::errs() << "Cannot find the variable: " << node->getName() << "\n";
    return nullptr;
  }

  builder.CreateStore(value, variable);
  return value;
}

llvm::Value* generateIRForVariableRef(const VariableRefAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::AllocaInst* variable = namedValues[node->getName()];
  if (!variable) {
    llvm::errs() << "Cannot find the variable: " << node->getName() << "\n";
    return nullptr;
  }
  return variable;
}

llvm::Value* generateIRForPrint(const PrintAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::Value* expressionValue = generateIR(node->getExpression(), builder, module, parentFunction);

  llvm::FunctionType* printfType = llvm::FunctionType::get(builder.getInt64Ty(), {builder.getInt8PtrTy()}, true);
  llvm::FunctionCallee printfFunc = module.getOrInsertFunction("printf", printfType);

  llvm::Value* formatStr = builder.CreateGlobalStringPtr("%ld\n");

  return builder.CreateCall(printfFunc, {formatStr, expressionValue});
}

llvm::Value* generateIRForIfNode(const IfNode* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::LLVMContext& context = builder.getContext();

  llvm::Value* conditionValue = generateIR(node->getCondition(), builder, module, parentFunction);
  if (!conditionValue) {
    llvm::errs() << "Failed to generate condition IR\n";
    return nullptr;
  }

  conditionValue = builder.CreateICmpNE(conditionValue, llvm::ConstantInt::get(conditionValue->getType(), 0), "ifcond");

  llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", parentFunction);
  llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(context, "else");
  llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "ifcont");

  builder.CreateCondBr(conditionValue, thenBB, elseBB);

  builder.SetInsertPoint(thenBB);
  for (const auto& thenNode : node->getThenBody()) {
    generateIR(thenNode.get(), builder, module, parentFunction);
  }
  builder.CreateBr(mergeBB);

  parentFunction->getBasicBlockList().push_back(elseBB);
  builder.SetInsertPoint(elseBB);
  for (const auto& elseNode : node->getElseBody()) {
    generateIR(elseNode.get(), builder, module, parentFunction);
  }
  builder.CreateBr(mergeBB);

  parentFunction->getBasicBlockList().push_back(mergeBB);
  builder.SetInsertPoint(mergeBB);

  return nullptr;
}

llvm::Value* generateIRForArithmeticOpNode(const ArithmeticOpNode* node,
                                           llvm::IRBuilder<>& builder,
                                           llvm::Module& module,
                                           llvm::Function* parentFunction) {
  llvm::Value* leftValue = generateIR(node->getLeft(), builder, module, parentFunction);
  llvm::Value* rightValue = generateIR(node->getRight(), builder, module, parentFunction);
  if (!leftValue || !rightValue) {
    llvm::errs() << "Failed to generate IR for operands of arithmetic operation\n";
    return nullptr;
  }
  switch (node->getOperator()) {
    case ArithmeticOpNode::Operator::ADD: return builder.CreateAdd(leftValue, rightValue, "addtmp");
    case ArithmeticOpNode::Operator::SUBTRACT: return builder.CreateSub(leftValue, rightValue, "subtmp");
    case ArithmeticOpNode::Operator::MULTIPLY: return builder.CreateMul(leftValue, rightValue, "multmp");
    case ArithmeticOpNode::Operator::DIVIDE:
      return builder.CreateSDiv(leftValue,
                                rightValue,
                                "divtmp");
    case ArithmeticOpNode::Operator::MODULO:
      return builder.CreateSRem(leftValue,
                                rightValue,
                                "modtmp");
  }
  return nullptr;
}

llvm::Value* generateIRForCompareOpNode(const CompareOpNode* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::Value* leftValue = generateIR(node->getLeft(), builder, module, parentFunction);
  llvm::Value* rightValue = generateIR(node->getRight(), builder, module, parentFunction);

  if (!leftValue || !rightValue) {
    llvm::errs() << "Failed to generate IR for operands of compare operation\n";
    return nullptr;
  }

  switch (node->getOperator()) {
    case CompareOpNode::Operator::LESS_THAN:return builder.CreateICmpSLT(leftValue, rightValue, "cmplttmp");
    case CompareOpNode::Operator::GREATER_THAN:return builder.CreateICmpSGT(leftValue, rightValue, "cmpgttmp");
    case CompareOpNode::Operator::LESS_THAN_OR_EQUAL:return builder.CreateICmpSLE(leftValue, rightValue, "cmplesstmp");
    case CompareOpNode::Operator::GREATER_THAN_OR_EQUAL:
      return builder.CreateICmpSGE(leftValue,
                                   rightValue,
                                   "cmpgeqtmp");
    case CompareOpNode::Operator::EQUALS:return builder.CreateICmpEQ(leftValue, rightValue, "cmpeqtmp");
  }
}

llvm::Value* generateIRForArrayDecl(const ArrayDeclAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::LLVMContext& context = module.getContext();

  llvm::Type* elementType;
  if (node->getElementType() == "int") {
    elementType = llvm::Type::getInt64Ty(context);
  } else if (node->getElementType() == "bool") {
    elementType = llvm::Type::getInt1Ty(context);
  } else {
    llvm::errs() << "Unsupported array element type: " << node->getElementType() << "\n";
    return nullptr;
  }

  llvm::ArrayType* arrayType = llvm::ArrayType::get(elementType, node->getSize());

  auto* arrayVar = new llvm::GlobalVariable(
      module,
      arrayType,
      false,
      llvm::GlobalValue::ExternalLinkage,
      nullptr,
      node->getName()
  );

  if (!node->getElements().empty()) {
    std::vector<llvm::Constant*> initialValues;
    for (int64_t value : node->getElements()) {
      initialValues.push_back(llvm::ConstantInt::get(context, llvm::APInt(64, value)));
    }

    llvm::Constant* initializer = llvm::ConstantArray::get(arrayType, initialValues);
    arrayVar->setInitializer(initializer);
  } else {
    arrayVar->setInitializer(llvm::ConstantAggregateZero::get(arrayType));
  }

  return arrayVar;
}

llvm::Value* generateIRForArrayAccess(const ArrayAccessAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::Value* indexValue = generateIR(node->getIndex(), builder, module, parentFunction);
  if (!indexValue) {
    llvm::errs() << "Failed to generate IR for array index\n";
    return nullptr;
  }

  llvm::GlobalVariable* arrayVar = module.getNamedGlobal(node->getArrayName());
  if (!arrayVar) {
    llvm::errs() << "Array not found: " << node->getArrayName() << "\n";
    return nullptr;
  }

  indexValue = builder.CreateIntCast(indexValue, llvm::Type::getInt64Ty(module.getContext()), true);

  std::vector<llvm::Value*> indices = {
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(module.getContext()), 0),
      indexValue
  };

  llvm::Type* arrayType = arrayVar->getValueType();
  llvm::Value* elementPtr = builder.CreateGEP(arrayType, arrayVar, indices, "arrayelem");

  return elementPtr;
}

llvm::Value* generateIRForAssignment(const AssignmentAST* node, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::Value* rhsValue = nullptr;
  rhsValue = generateIR(node->getRHS(), builder, module, parentFunction);

  if (!rhsValue) {
    throw std::runtime_error("Failed to generate RHS value for assignment");
  }

  llvm::Value* lhsLocation = nullptr;

  if (auto* varRefAST = dynamic_cast<const VariableRefAST*>(node->getLHS())) {
    lhsLocation = generateIRForVariableRef(varRefAST, builder, module, parentFunction);
  } else if (auto* arrayAccess = dynamic_cast<const ArrayAccessAST*>(node->getLHS())) {
    lhsLocation = generateIRForArrayAccess(arrayAccess, builder, module, parentFunction);
  } else {
    throw std::runtime_error("Left-hand side of assignment is not assignable");
  }

  if (lhsLocation) {
    builder.CreateStore(rhsValue, lhsLocation);
  }

  return rhsValue;
}

llvm::Value* generateIR(const ASTNode* node,
                        llvm::IRBuilder<>& builder,
                        llvm::Module& module,
                        llvm::Function* parentFunction) {
  if (auto boolNode = dynamic_cast<const BooleanAST*>(node)) {
    return generateIRForBoolean(boolNode, builder);
  }
  if (auto numNode = dynamic_cast<const NumberAST*>(node)) {
    return generateIRForNumber(numNode, builder);
  }
  if (auto varDeclNode = dynamic_cast<const VariableDeclAST*>(node)) {
    return generateIRForVariableDecl(varDeclNode, builder, module, parentFunction);
  }
  if (auto varAssignNode = dynamic_cast<const VariableAssignAST*>(node)) {
    return generateIRForVariableAssign(varAssignNode, builder, module, parentFunction);
  }
  if (auto varRefNode = dynamic_cast<const VariableRefAST*>(node)) {
    llvm::Value* valuePtr = generateIRForVariableRef(varRefNode, builder, module, parentFunction);
    return builder.CreateLoad(builder.getInt64Ty(), valuePtr, "loadtmp");
  }
  if (auto printNode = dynamic_cast<const PrintAST*>(node)) {
    return generateIRForPrint(printNode, builder, module, parentFunction);
  }
  if (auto forNode = dynamic_cast<const ForNode*>(node)) {
    return generateIRForForNode(forNode, builder, module, parentFunction);
  }
  if (auto ifNode = dynamic_cast<const IfNode*>(node)) {
    return generateIRForIfNode(ifNode, builder, module, parentFunction);
  }
  if (auto arithmeticNode = dynamic_cast<const ArithmeticOpNode*>(node)) {
    return generateIRForArithmeticOpNode(arithmeticNode, builder, module, parentFunction);
  }
  if (auto arithmeticNode = dynamic_cast<const CompareOpNode*>(node)) {
    return generateIRForCompareOpNode(arithmeticNode, builder, module, parentFunction);
  }
  if (auto arrayDeclNode = dynamic_cast<const ArrayDeclAST*>(node)) {
    return generateIRForArrayDecl(arrayDeclNode, builder, module, parentFunction);
  }
  if (auto arrayAccessNode = dynamic_cast<const ArrayAccessAST*>(node)) {
    llvm::Value* valuePtr = generateIRForArrayAccess(arrayAccessNode, builder, module, parentFunction);
    return builder.CreateLoad(builder.getInt64Ty(), valuePtr, "loadelem");
  }
  if (auto assigmentNode = dynamic_cast<const AssignmentAST*>(node)) {
    return generateIRForAssignment(assigmentNode, builder, module, parentFunction);
  }

  return nullptr;
}

llvm::Value* generateIRForForNode(const ForNode* forNode, llvm::IRBuilder<>& builder, llvm::Module& module, llvm::Function* parentFunction) {
  llvm::LLVMContext& context = builder.getContext();

  llvm::Value* start = generateIR(forNode->getStart(), builder, module, parentFunction);
  llvm::Value* finish = generateIR(forNode->getFinish(), builder, module, parentFunction);
  llvm::Value* step = generateIR(forNode->getStep(), builder, module, parentFunction);

  llvm::BasicBlock* preheaderBB = builder.GetInsertBlock();
  llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context, "loop", parentFunction);
  llvm::BasicBlock* afterLoopBB = llvm::BasicBlock::Create(context, "after_loop", parentFunction);

  llvm::AllocaInst* alloca = builder.CreateAlloca(start->getType(), nullptr, forNode->getIteratorName());
  namedValues[forNode->getIteratorName()] = alloca;

  builder.CreateBr(loopBB);
  builder.SetInsertPoint(loopBB);

  llvm::PHINode* phiNode = builder.CreatePHI(start->getType(), 2, forNode->getIteratorName());
  phiNode->addIncoming(start, preheaderBB);

  builder.CreateStore(phiNode, alloca);

  for (const auto& bodyNode : forNode->getBody()) {
    generateIR(bodyNode.get(), builder, module, parentFunction);
  }

  llvm::Value* currentVar = builder.CreateLoad(start->getType(), alloca);
  llvm::Value* nextVar = builder.CreateAdd(currentVar, step, "nextvar");
  llvm::Value* endCond = builder.CreateICmpSLT(nextVar, finish, "loopcond");

  builder.CreateCondBr(endCond, loopBB, afterLoopBB);
  phiNode->addIncoming(nextVar, builder.GetInsertBlock());

  builder.SetInsertPoint(afterLoopBB);

  return nullptr;
}

std::unique_ptr<llvm::Module> generateModuleIR(std::vector<std::unique_ptr<ASTNode>>& astNodes, llvm::LLVMContext& context, bool withOpt) {
  auto module = std::make_unique<llvm::Module>("my_module", context);
  llvm::IRBuilder<> builder(context);

  llvm::FunctionType* funcType = llvm::FunctionType::get(builder.getVoidTy(), false);
  llvm::Function* mainFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", *module);
  llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
  builder.SetInsertPoint(entry);

  for (auto& node : astNodes) {
    generateIR(node.get(), builder, *module, mainFunc);
  }
  builder.CreateRetVoid();

  llvm::legacy::PassManager passManager;
  passManager.add(llvm::createPromoteMemoryToRegisterPass());
  passManager.add(llvm::createInstructionCombiningPass());
  passManager.add(llvm::createReassociatePass());
  passManager.add(llvm::createGVNPass());
  passManager.add(llvm::createCFGSimplificationPass());

  if (withOpt) passManager.run(*module);

  std::string irCode;
  llvm::raw_string_ostream stream(irCode);
  module->print(stream, nullptr);

  std::string outputFilename = withOpt ? "output_opt.ll" : "output_native.ll";
  std::ofstream outputFile(outputFilename);
  // std::cout << irCode;
  outputFile << irCode;
  outputFile.close();

  return module;
}