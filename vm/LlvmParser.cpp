#include "LlvmParser.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <cassert>

LLVMParser::LLVMParser(const std::string& code)
    : codeStream(code), currentFunction(nullptr), currentBlock(nullptr) {}

void LLVMParser::parse() {
  std::string line;
  while (std::getline(codeStream, line)) {
    line = trim(line);
    if (line.empty()) continue;

    if (line[0] == ';') {
      moduleInfo.push_back(line);
    } else if (line.substr(0, 7) == "source_") {
      size_t pos = line.find('=');
      if (pos != std::string::npos) {
        sourceFilename = line.substr(pos + 2);
      }
    } else if (line[0] == '@') {
      parseGlobalVariable(line);
    } else if (line.substr(0, 6) == "define") {
      parseFunction(line);
    } else if (line == "}" || line.substr(0, 7) == "declare") {
      if (line == "}") {
        currentFunction = nullptr; // Завершаем функцию
        currentBlock = nullptr;    // Завершаем текущий блок
      }
    } else if (line.back() == ':') { // Метка блока
      parseBlock(line);
    } else {
      parseInstruction(line);
    }
  }
}

void LLVMParser::parseGlobalVariable(const std::string& line) {
  std::regex globalVarRegex(R"(^@(\w+)\s*=\s*.*\s*constant\s*(\[.*\])\s*c\"(.*)\")");
  std::smatch match;
  if (std::regex_search(line, match, globalVarRegex)) {
    globalVariables.push_back({match[1], match[2], match[3]});
  }
}

void LLVMParser::parseFunction(const std::string& line) {
  std::regex funcRegex(R"(^define\s+\w+\s+@(\w+)\()");
  std::smatch match;
  if (std::regex_search(line, match, funcRegex)) {
    functions.push_back({match[1], {}});
    currentFunction = &functions.back();
  }
}

void LLVMParser::parseBlock(const std::string& line) {
  if (currentFunction) {
    std::string blockName = line.substr(0, line.size() - 1);  // Убираем ':'
    currentFunction->blocks.push_back({blockName, {}});
    currentBlock = &currentFunction->blocks.back();
  }
}

void LLVMParser::parseInstruction(const std::string& line) {
  if (currentBlock) {
    size_t pos = line.find(' ');
    std::string type = (pos != std::string::npos) ? line.substr(0, pos) : line;
    currentBlock->instructions.push_back({type, line});
  }
}

std::string LLVMParser::trim(const std::string& str) {
  size_t first = str.find_first_not_of(' ');
  if (first == std::string::npos) return "";
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}

const std::vector<Function>& LLVMParser::getFunctions() const {
  return functions;
}

const std::vector<std::string>& LLVMParser::getModuleInfo() const {
  return moduleInfo;
}

const std::string& LLVMParser::getSourceFilename() const {
  return sourceFilename;
}

const std::vector<GlobalVariable>& LLVMParser::getGlobalVariables() const {
  return globalVariables;
}

void LLVMParser::printDebugInfo() const {
  std::cout << "Module Info:" << std::endl;
  for (const auto& line : moduleInfo) {
    std::cout << "  " << line << std::endl;
  }
  std::cout << "Source Filename: " << sourceFilename << std::endl;
  printGlobalVariables();
  printFunctions();
}

void LLVMParser::printFunctions() const {
  for (const auto& function : functions) {
    std::cout << "Function: " << function.name << std::endl;
    for (const auto& block : function.blocks) {
      std::cout << "  Block: " << block.name << std::endl;
      for (const auto& instr : block.instructions) {
        std::cout << "    Instruction: " << instr.type << " - " << instr.content << std::endl;
      }
    }
  }
}

void LLVMParser::printGlobalVariables() const {

  std::cout << "Global Variables:" << std::endl;
  for (const auto& variable : globalVariables) {
    std::cout << "  Name: " << variable.name << ", Type: " << variable.type << ", Value: " << variable.value
              << std::endl;
  }
}
