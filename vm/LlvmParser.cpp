#include <iostream>
#include <sstream>
#include <regex>
#include "LlvmIrParser.h"

// Функция для преобразования строки в Opcode
Opcode opcodeFromString(const std::string& str) {
  if (str == "phi") return Opcode::Phi;
  if (str == "mul") return Opcode::Mul;
  if (str == "add") return Opcode::Add;
  if (str == "icmp") return Opcode::ICmp;
  if (str == "br") return Opcode::Br;
  if (str == "call") return Opcode::Call;
  if (str == "ret") return Opcode::Ret;
  return Opcode::Unknown;
}

// Instruction Implementation

Instruction::Instruction(Opcode op, const std::vector<std::string>& arguments, const std::string& res)
    : opcode(op), args(arguments), result(res) {}

void Instruction::print() const {
  std::cout << "Instruction(opcode='";

  switch (opcode) {
    case Opcode::Phi: std::cout << "phi";
      break;
    case Opcode::Mul: std::cout << "mul";
      break;
    case Opcode::Add: std::cout << "add";
      break;
    case Opcode::ICmp: std::cout << "icmp";
      break;
    case Opcode::Br: std::cout << "br";
      break;
    case Opcode::Call: std::cout << "call";
      break;
    case Opcode::Ret: std::cout << "ret";
      break;
    default: std::cout << "unknown";
      break;
  }

  std::cout << "', args=[";
  for (size_t i = 0; i < args.size(); ++i) {
    std::cout << args[i];
    if (i < args.size() - 1) std::cout << ", ";
  }
  std::cout << "], result='" << result << "')\n";
}

// LLVMIRParser Implementation

void LLVMIRParser::parse(const std::string& llvm_ir) {
  std::istringstream stream(llvm_ir);
  std::string line;
  std::regex label_regex(R"((\w+):)");
  std::regex instr_regex(R"((\S+)\s*=\s*(\S+)\s*(.*))");
  std::regex simple_instr_regex(R"((\S+)\s*(.*))");

  while (std::getline(stream, line)) {
    line = trim(line);

    // Пропускаем пустые строки и комментарии
    if (line.empty() || line[0] == ';') continue;

    // Проверяем на метки
    std::smatch label_match;
    if (std::regex_match(line, label_match, label_regex)) {
      std::string label = label_match[1].str();
      labels[label] = instructions.size();
      continue;
    }

    // Парсим инструкции
    std::smatch instr_match;
    if (std::regex_match(line, instr_match, instr_regex)) {
      std::string result = instr_match[1].str();
      Opcode opcode = opcodeFromString(instr_match[2].str());
      std::vector<std::string> args = split(instr_match[3].str());
      instructions.emplace_back(opcode, args, result);
    } else if (std::regex_match(line, instr_match, simple_instr_regex)) {
      Opcode opcode = opcodeFromString(instr_match[1].str());
      std::vector<std::string> args = split(instr_match[2].str());
      instructions.emplace_back(opcode, args);
    }
  }
}

void LLVMIRParser::printInstructions() const {
  for (const auto& instr : instructions) {
    instr.print();
  }
}

void LLVMIRParser::printLabels() const {
  for (const auto& label : labels) {
    std::cout << label.first << ": " << label.second << "\n";
  }
}

std::string LLVMIRParser::trim(const std::string& str) {
  const std::string whitespace = " \t";
  const size_t begin = str.find_first_not_of(whitespace);
  if (begin == std::string::npos) {
    return "";
  }

  const size_t end = str.find_last_not_of(whitespace);
  const size_t range = end - begin + 1;
  return str.substr(begin, range);
}

std::vector<std::string> LLVMIRParser::split(const std::string& str) {
  std::vector<std::string> tokens;
  std::istringstream stream(str);
  std::string token;
  while (std::getline(stream, token, ',')) {
    tokens.push_back(trim(token));
  }
  return tokens;
}