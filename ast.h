#include <memory>
#include <string>
#include <vector>

class ASTNode {
 public:
  virtual ~ASTNode() = default;
};

class VariableDeclAST : public ASTNode {
 public:
  VariableDeclAST(const std::string& type, const std::string& name, int64_t value)
      : type(type), name(name), value(value) {}

  const std::string& getType() const { return type; }
  const std::string& getName() const { return name; }
  int64_t getValue() const { return value; }

 private:
  std::string type;
  std::string name;
  int64_t value;
};

class PrintAST : public ASTNode {
 public:
  PrintAST(const std::string& identifier) : identifier(identifier) {}

  const std::string& getIdentifier() const { return identifier; }

 private:
  std::string identifier;
};