#ifndef LLVM_PARSER_H
#define LLVM_PARSER_H

#include <string>
#include <vector>
#include <sstream>  // Добавить этот include

struct GlobalVariable {
  std::string name;
  std::string type;
  std::string value;
};

struct Instruction {
  std::string type;
  std::string content;
};

struct Block {
  std::string name;
  std::vector<Instruction> instructions;
};

struct Function {
  std::string name;
  std::vector<Block> blocks;
};

class LLVMParser {
 public:
  explicit LLVMParser(const std::string& code);
  void parse();
  const std::vector<Function>& getFunctions() const;
  const std::vector<std::string>& getModuleInfo() const;
  const std::string& getSourceFilename() const;
  const std::vector<GlobalVariable>& getGlobalVariables() const;
  void printDebugInfo() const;
  void printFunctions() const;
  void printGlobalVariables() const;

 private:
  std::istringstream codeStream;
  std::vector<Function> functions;
  std::vector<std::string> moduleInfo;
  std::vector<GlobalVariable> globalVariables;
  std::string sourceFilename;
  Function* currentFunction;
  Block* currentBlock;

  void parseGlobalVariable(const std::string& line);
  void parseFunction(const std::string& line);
  void parseBlock(const std::string& line);
  void parseInstruction(const std::string& line);

  static std::string trim(const std::string& str);
};

#endif // LLVM_PARSER_H