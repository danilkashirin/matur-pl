#ifndef LLVMIRPARSER_H
#define LLVMIRPARSER_H

#include <vector>
#include <string>
#include <unordered_map>

// Перечисления для Opcode
enum class Opcode {
  Phi,
  Mul,
  Add,
  ICmp,
  Br,
  Call,
  Ret,
  Unknown
};

// По строке возвращает константу из enum Opcode
Opcode opcodeFromString(const std::string& str);

class Instruction {
 public:
  Opcode opcode;
  std::vector<std::string> args;
  std::string result;

  Instruction(Opcode op, const std::vector<std::string>& arguments, const std::string& res = "");

  void print() const;
};

class LLVMIRParser {
 public:
  void parse(const std::string& llvm_ir);
  void printInstructions() const;
  void printLabels() const;

 private:
  std::vector<Instruction> instructions;
  std::unordered_map<std::string, size_t> labels;

  static std::string trim(const std::string& str);
  static std::vector<std::string> split(const std::string& str);
};

#endif // LLVMIRPARSER_H