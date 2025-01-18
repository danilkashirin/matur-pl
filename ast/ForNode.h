#ifndef FOR_NODE_H
#define FOR_NODE_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <cassert>
#include "ASTNode.h"

class ForNode : public ASTNode {
 public:
  ForNode(std::string iteratorName,
          ASTNode* start,
          ASTNode* finish,
          ASTNode* step,
          std::vector<std::unique_ptr<ASTNode>> body)
      : iteratorName_(std::move(iteratorName)), start_(start), finish_(finish), step_(step), body_(std::move(body)) {}

  ~ForNode() override {
    delete start_;
    delete finish_;
    delete step_;
  }

  [[nodiscard]] const std::string& getIteratorName() const { return iteratorName_; }
  [[nodiscard]] ASTNode* getStart() const { return start_; }
  [[nodiscard]] ASTNode* getFinish() const { return finish_; }
  [[nodiscard]] ASTNode* getStep() const { return step_; }
  [[nodiscard]] const std::vector<std::unique_ptr<ASTNode>>& getBody() const { return body_; }

  std::vector<std::string> getVariableNames() override {
    assert(this->start_ != nullptr);
    assert(this->finish_ != nullptr);
    assert(this->step_ != nullptr);
    std::vector<std::string> start_names = this->start_->getVariableNames();
    std::vector<std::string> finish_names = this->finish_->getVariableNames();
    std::vector<std::string> step_names = this->step_->getVariableNames();
    std::size_t body_nodes_count = 0;
    for (auto& body_node : this->body_) {
        assert(body_node.get() != nullptr);
        body_nodes_count += body_node->getVariableNames().size();
    }
    std::vector<std::string> values(
        start_names.size()
        + finish_names.size()
        + step_names.size()
        + body_nodes_count
    );
    std::size_t i = 0;
    while (i < start_names.size()) {
        values[i] = start_names[i];
        ++i;
    }
    while (i < start_names.size() + finish_names.size()) {
        values[i] = finish_names[i];
        ++i;
    }
    while (i < values.size() - body_nodes_count) {
        values[i] = step_names[i];
        ++i;
    }
    for (auto& body_node : this->body_) {
        assert(body_node.get() != nullptr);
        for (std::string name : body_node->getVariableNames()) {
            values[i] = name;
        }
    }
    return values;
  }


 private:
  std::string iteratorName_;
  ASTNode* start_;
  ASTNode* finish_;
  ASTNode* step_;
  std::vector<std::unique_ptr<ASTNode>> body_;
};

#endif // FOR_NODE_H