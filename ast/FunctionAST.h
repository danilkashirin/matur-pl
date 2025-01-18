#ifndef FUNCTION_NODE_H
#define FUNCTION_NODE_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "ASTNode.h"

class FunctionNode : public ASTNode {
 public:
  FunctionNode(std::string function,
               std::vector<std::unique_ptr<ASTNode>> parameters,
               std::vector<std::unique_ptr<ASTNode>> body)
      : function_name(std::move(function)), body_(std::move(body)), parameters_(std::move(parameters)) {}

  [[nodiscard]] const std::string& getFunctionName() const { return function_name; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getBody() const { return body_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getParameters() const { return parameters_; }

  std::vector<std::string> getVariableNames() override {
    std::size_t body_nodes_count = 0;
    for (auto& body_node : this->body_) {
      assert(body_node.get() != nullptr);
      body_nodes_count += body_node->getVariableNames().size();
    }
    for (auto& parameters_node : this->parameters_) {
      assert(parameters_node.get() != nullptr);
      body_nodes_count += parameters_node->getVariableNames().size();
    }
    std::vector<std::string> values(body_nodes_count);

    size_t i = 0;

    for (auto& body_node : this->body_) {
      for (std::string name : body_node->getVariableNames()) {
        values[i] = name;
        i++;
      }
    }

    for (auto& parameters_node : this->parameters_) {
      assert(parameters_node.get() != nullptr);
      for (std::string name : parameters_node->getVariableNames()) {
        values[i] = name;
        i++;
      }
    }
    return values;
  }

 private:
  std::string function_name;
  std::vector<std::unique_ptr<ASTNode>> parameters_;
  std::vector<std::unique_ptr<ASTNode>> body_;
};

#endif // FUNCTION_NODE_H