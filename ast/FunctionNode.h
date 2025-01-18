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

private:
    std::string function_name;
    std::vector<std::unique_ptr<ASTNode>> parameters_;
    std::vector<std::unique_ptr<ASTNode>> body_;
};

#endif // FUNCTION_NODE_H