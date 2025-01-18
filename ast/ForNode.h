#ifndef FOR_NODE_H
#define FOR_NODE_H

#include <string>
#include "ASTNode.h"

class ForNode : public ASTNode {
public:
    ForNode(const std::string& iteratorName, ASTNode* start, ASTNode* finish, ASTNode* step, ASTNode* body)
        : iteratorName_(iteratorName), start_(start), finish_(finish), step_(step), body_(body) {}

    ~ForNode() override {
        delete start_;
        delete finish_;
        delete step_;
        delete body_;
    }

    [[nodiscard]] const std::string& getIteratorName() const { return iteratorName_; }
    [[nodiscard]] ASTNode* getStart() const { return start_; }
    [[nodiscard]] ASTNode* getFinish() const { return finish_; }
    [[nodiscard]] ASTNode* getStep() const { return step_; }
    [[nodiscard]] ASTNode* getBody() const { return body_; }

private:
    std::string iteratorName_;
    ASTNode* start_;
    ASTNode* finish_;
    ASTNode* step_;
    ASTNode* body_;
};

#endif // FOR_NODE_H