#include "VirtualMachine.h"
#include <stack>

VirtualMachine::VirtualMachine()
    : operationCount(0), gc(storage, stack, current_name_scope) {}

std::unordered_map<std::string, Value>& VirtualMachine::getStorage() {
  return storage;
};

std::vector<int64_t>& VirtualMachine::getStack() {
  return stack;
};

void VirtualMachine::execute(const std::vector<std::tuple<std::string, std::vector<int64_t>>>& bytecode) {
  auto start = std::chrono::high_resolution_clock::now();
  size_t pc = 0;
  std::vector<size_t> callStack;
  std::unordered_map<std::string, size_t> functionTable;

  while (pc < bytecode.size()) {
    const auto& [operation, operands] = bytecode[pc];

    ++operationCount;
    if (operationCount % 100 == 0) {
      gc.collect();
    }

    if (operation == "LOAD_CONST") {
      loadConst(operands);
    } else if (operation == "DECLARE_VAR") {
      declareVar(operands);
    } else if (operation == "ASSIGN_VAR") {
      assignVar(operands);
    } else if (operation == "LOAD_VAR") {
      loadVar(operands);
    } else if (operation == "DECLARE_ARRAY") {
      declareArray(operands);
    } else if (operation == "ASSIGN_ARRAY_ELEMENT") {
      assignArrayElement(operands);
    } else if (operation == "LOAD_ARRAY_ELEMENT") {
      loadArrayElement(operands);
    } else if (operation == "ADD") {
      add();
    } else if (operation == "SUBTRACT") {
      subtract();
    } else if (operation == "MULTIPLY") {
      multiply();
    } else if (operation == "DIVIDE") {
      divide();
    } else if (operation == "MODULO") {
      modulo();
    } else if (operation == "EQUALS") {
      equals();
    } else if (operation == "LESS_THAN") {
      lessThan();
    } else if (operation == "GREATER_THAN") {
      greaterThan();
    } else if (operation == "LESS_THAN_OR_EQUAL") {
      lessThanOrEqual();
    } else if (operation == "GREATER_THAN_OR_EQUAL") {
      greaterThanOrEqual();
    } else if (operation == "JUMP") {
      pc = operands[0];
      continue;
    } else if (operation == "JUMP_IF_FALSE") {
      if (stack.empty()) {
        std::cerr << "JUMP_IF_FALSE failed: stack is empty\n";
        return;
      }
      int64_t condition = stack.back();
      stack.pop_back();
      if (condition == 0) {
        pc = operands[0];
        continue;
      }
    } else if (operation == "FUNC_DEF") {
      std::string funcName(operands.begin() + 1, operands.end() - 1);
      auto skip = operands.back();
      functionTable[funcName] = pc + 1;
      pc += skip;
    } else if (operation == "CALL_FUNC") {
      std::string funcName(operands.begin() + 1, operands.end());

      if (functionTable.find(funcName) == functionTable.end()) {
        std::cerr << "Function " << funcName << " not found\n";
        return;
      }

      callStack.push_back(pc);
      current_name_scope.push(storage);
      pc = functionTable[funcName];
      continue;
    } else if (operation == "RETURN") {
      if (callStack.empty() || current_name_scope.empty()) {
        std::cerr << "RETURN failed: empty call stack or scope stack\n";
        return;
      }

      pc = callStack.back();
      callStack.pop_back();
      storage = current_name_scope.top();
      current_name_scope.pop();
    } else if (operation == "PRINT") {
      print();
    } else {
      std::cerr << "Unknown operation: " << operation << "\n";
    }

    ++pc;
  }

  gc.cleanup();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
}

void VirtualMachine::assignVar(const std::vector<int64_t>& operands) {
  if (operands.size() < 2) {
    std::cerr << "Invalid assign operation: insufficient operands\n";
    return;
  }

  std::string varName;
  int64_t nameSize = operands[0];
  for (auto i = 1; i <= nameSize; ++i) {
    varName += static_cast<char>(operands[i]);
  }

  if (!stack.empty()) {
    int64_t value = stack.back();
    stack.pop_back();
    auto it = storage.find(varName);
    if (it != storage.end()) {
      storage[varName] = value;
    } else {
      std::cerr << "Variable not found: " << varName << "\n";
    }
  } else {
    std::cerr << "Assign operation failed: stack is empty\n";
  }
}

void VirtualMachine::performArithmeticOperation(const std::function<int64_t(int64_t, int64_t)>& op) {
  if (stack.size() < 2) {
    std::cerr << "Arithmetic operation failed: insufficient operands on stack\n";
    return;
  }

  int64_t b = stack.back();
  stack.pop_back();
  int64_t a = stack.back();
  stack.pop_back();
  stack.push_back(op(a, b));
}

void VirtualMachine::add() {
  performArithmeticOperation([](int64_t a, int64_t b) { return a + b; });
}

void VirtualMachine::subtract() {
  performArithmeticOperation([](int64_t a, int64_t b) { return a - b; });
}

void VirtualMachine::multiply() {
  performArithmeticOperation([](int64_t a, int64_t b) { return a * b; });
}

void VirtualMachine::divide() {
  performArithmeticOperation([](int64_t a, int64_t b) { return a / b; });
}

void VirtualMachine::modulo() {
  performArithmeticOperation([](int64_t a, int64_t b) { return a % b; });
}

void VirtualMachine::lessThan() {
  performComparisonOperation([](int64_t a, int64_t b) { return a < b; });
}

void VirtualMachine::greaterThan() {
  performComparisonOperation([](int64_t a, int64_t b) { return a > b; });
}

void VirtualMachine::lessThanOrEqual() {
  performComparisonOperation([](int64_t a, int64_t b) { return a <= b; });
}

void VirtualMachine::greaterThanOrEqual() {
  performComparisonOperation([](int64_t a, int64_t b) { return a >= b; });
}

void VirtualMachine::equals() {
  performComparisonOperation([](int64_t a, int64_t b) { return a == b; });
}

void VirtualMachine::performComparisonOperation(const std::function<bool(int64_t, int64_t)>& op) {
  if (stack.size() < 2) {
    std::cerr << "Comparison operation failed: insufficient operands on stack\n";
    return;
  }

  int64_t b = stack.back();
  stack.pop_back();
  int64_t a = stack.back();
  stack.pop_back();
  stack.push_back(op(a, b) ? 1 : 0);
}

void VirtualMachine::loadConst(const std::vector<int64_t>& operands) {
  if (!operands.empty()) {
    stack.push_back(operands[0]);
  } else {
    std::cerr << "LoadConst operation failed: no operands\n";
  }
}

void VirtualMachine::declareVar(const std::vector<int64_t>& operands) {
  if (operands.size() < 2) return;

  std::string varName;
  int64_t nameSize = operands[0];

  for (auto i = 1; i <= nameSize; ++i) {
    varName += static_cast<char>(operands[i]);
  }

  if (!stack.empty()) {
    int64_t value = stack.back();
    stack.pop_back();
    storage[varName] = value;
  } else {
    std::cerr << "DeclareVar operation failed: stack is empty\n";
  }
}

void VirtualMachine::loadVar(const std::vector<int64_t>& operands) {
  if (operands.size() < 2) return;

  std::string varName;
  int64_t nameSize = operands[0];

  for (auto i = 1; i <= nameSize; ++i) {
    varName += static_cast<char>(operands[i]);
  }

  auto it = storage.find(varName);
  if (it != storage.end()) {
    if (auto* val = std::get_if<int64_t>(&it->second)) {
      stack.push_back(*val);
    } else {
      std::cerr << "Variable is not a scalar value: " << varName << "\n";
    }
  } else {
    std::cerr << "Variable not found: " << varName << "\n";
  }
}

void VirtualMachine::print() {
  if (!stack.empty()) {
    int64_t value = stack.back();
    stack.pop_back();
    std::cout << value << "\n";
  } else {
    std::cerr << "Print operation failed: stack is empty\n";
  }
}

void VirtualMachine::declareArray(const std::vector<int64_t>& operands) {
  if (operands.size() < 2) {
    std::cerr << "Invalid array declaration: insufficient operands\n";
    return;
  }

  int64_t size = operands[0];
  std::string arrayName;
  int64_t nameSize = operands[1];

  for (auto i = 2; i < 2 + nameSize; ++i) {
    arrayName += static_cast<char>(operands[i]);
  }

  if (storage.find(arrayName) != storage.end()) {
    std::cerr << "Array already declared or variable already exists with the name: " << arrayName << "\n";
    return;
  }

  std::vector<int64_t> arrayValues;
  size_t valuesStart = 2 + nameSize;

  for (size_t i = valuesStart; i < operands.size(); ++i) {
    arrayValues.push_back(operands[i]);
  }

  while (arrayValues.size() < static_cast<size_t>(size)) {
    arrayValues.push_back(0);
  }

  storage[arrayName] = arrayValues;
}

void VirtualMachine::assignArrayElement(const std::vector<int64_t>& operands) {
  if (stack.empty()) {
    std::cerr << "Stack is empty for value\n";
    return;
  }

  int64_t value = stack.back();
  stack.pop_back();

  if (stack.empty()) {
    std::cerr << "Stack is empty for index\n";
    return;
  }

  int64_t index = stack.back();
  stack.pop_back();

  if (operands.size() < 2) {
    std::cerr << "Invalid ASSIGN_ARRAY_ELEMENT operation: insufficient operands\n";
    return;
  }

  std::string arrayName;
  int64_t nameSize = operands[0];
  for (auto i = 1; i <= nameSize; ++i) {
    arrayName += static_cast<char>(operands[i]);
  }

  auto it = storage.find(arrayName);
  if (it == storage.end()) {
    std::cerr << "Array not found: " << arrayName << "\n";
    return;
  }

  if (auto* arr = std::get_if<std::vector<int64_t>>(&it->second)) {
    if (index < 0 || index >= static_cast<int64_t>(arr->size())) {
      std::cerr << "Index out of bounds for array: " << arrayName << " index: " << index << "\n";
      return;
    }
    (*arr)[index] = value;
  } else {
    std::cerr << "Variable is not an array: " << arrayName << "\n";
  }
}

void VirtualMachine::loadArrayElement(const std::vector<int64_t>& operands) {
  if (stack.empty()) {
    std::cerr << "Stack is empty\n";
    return;
  }

  int64_t index = stack.back();
  stack.pop_back();

  if (operands.size() < 2) {
    std::cerr << "Invalid array element load: insufficient operands\n";
    return;
  }

  std::string arrayName;
  int64_t nameSize = operands[0];

  for (auto i = 1; i < 1 + nameSize; ++i) {
    arrayName += static_cast<char>(operands[i]);
  }

  auto it = storage.find(arrayName);
  if (it == storage.end()) {
    std::cerr << "Array not declared: " << arrayName << "\n";
    return;
  }

  if (auto* arr = std::get_if<std::vector<int64_t>>(&it->second)) {
    if (index < 0 || index >= static_cast<int64_t>(arr->size())) {
      std::cerr << "Array index out of bounds: " << index << "\n";
      return;
    }

    stack.push_back((*arr)[index]);
  } else {
    std::cerr << "Variable is not an array: " << arrayName << "\n";
  }
}
