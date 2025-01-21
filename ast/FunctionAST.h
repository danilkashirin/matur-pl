#ifndef MATUR_PL_AST_FUNCTIONAST_H_
#define MATUR_PL_AST_FUNCTIONAST_H_

#include <string>
#include <vector>
#include <memory>
#include "ASTNode.h"

class FunctionDeclNode : public ASTNode {
 public:
  FunctionDeclNode(std::string function_name,
                   std::vector<std::string> parameters,
                   std::vector<std::unique_ptr<ASTNode>> body)
      : function_name_(std::move(function_name)),
        parameters_(std::move(parameters)),
        body_(std::move(body)) {}

  [[nodiscard]] const std::string& getFunctionName() const { return function_name_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getBody() const { return body_; }
  [[nodiscard]] const std::vector<std::string>& getParameters() const { return parameters_; }

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t offset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;
    std::vector<std::tuple<std::string, std::vector<int64_t>>> finish_bytecode;

    std::vector<int64_t> operands;



    for (const auto & parameter : parameters_) {
      operands.clear();
      operands.push_back(static_cast<int64_t>(parameter.size()));
      for (char c : parameter) {
        operands.push_back(static_cast<int64_t>(c));
      }
      bytecode.emplace_back("DECLARE_VAR",  operands);
    }


    size_t body_offset = offset + bytecode.size() + 1;
    for (const auto& stmt : body_) {
      auto stmtBytecode = stmt->generateBytecode(body_offset);
      body_offset += stmtBytecode.size();
      bytecode.insert(bytecode.end(), stmtBytecode.begin(), stmtBytecode.end());
    }

    if (!bytecode.empty() && std::get<0>(bytecode.back()) != "RETURN") {
      bytecode.emplace_back("RETURN", std::vector<int64_t>{});
    }
    operands.clear();
    operands.push_back(static_cast<int64_t>(function_name_.size()));
    for (char c : function_name_) {
      operands.push_back(static_cast<int64_t>(c));
    }
    operands.push_back(static_cast<int64_t>(bytecode.size()));
    finish_bytecode.emplace_back("FUNC_DEF", operands);
    finish_bytecode.insert(finish_bytecode.end(), bytecode.begin(), bytecode.end());
    return finish_bytecode;

  }

 private:
  std::string function_name_;
  std::vector<std::string> parameters_;
  std::vector<std::unique_ptr<ASTNode>> body_;
};

class ReturnNode : public ASTNode {
 public:
  explicit ReturnNode(ASTNode* expression)
      : expression_(expression) {}

  [[nodiscard]] const ASTNode* getExpression() const { return expression_; }

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t offset) const override {
    auto bytecode = expression_->generateBytecode(offset);
    bytecode.emplace_back("RETURN", std::vector<int64_t>{});
    return bytecode;
  }

 private:
  ASTNode* expression_;
};

class FunctionCallNode : public ASTNode {
 public:
  FunctionCallNode(std::string function_name,
                   std::vector<std::unique_ptr<ASTNode>> arguments)
      : function_name_(std::move(function_name)),
        arguments_(std::move(arguments)) {}

  [[nodiscard]] const std::string& getFunctionName() const { return function_name_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getArguments() const { return arguments_; }

  std::vector<std::tuple<std::string, std::vector<int64_t>>> generateBytecode(size_t offset) const override {
    std::vector<std::tuple<std::string, std::vector<int64_t>>> bytecode;
    std::vector<int64_t> operands;
    for (auto it = arguments_.rbegin(); it != arguments_.rend(); ++it) {
      auto argBytecode = it->get()->generateBytecode(offset + bytecode.size());
      bytecode.insert(bytecode.end(), argBytecode.begin(), argBytecode.end());
    }
    operands.push_back(static_cast<int64_t>(function_name_.size()));
    for (char c : function_name_) {
      operands.push_back(static_cast<int64_t>(c));
    }
    bytecode.emplace_back("CALL_FUNC", operands);

    return bytecode;
  }

 private:
  std::string function_name_;
  std::vector<std::unique_ptr<ASTNode>> arguments_;
};

#endif //MATUR_PL_AST_FUNCTIONAST_H_
