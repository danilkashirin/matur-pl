#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include <functional>
#include <variant>
#include "GarbageCollector.h"

using Value = std::variant<int64_t, std::vector<int64_t>>;

class VirtualMachine {
 public:
  VirtualMachine();

  void execute(const std::vector<std::tuple<std::string, std::vector<int64_t>>>& bytecode);

  std::unordered_map<std::string, Value>& getStorage();
  std::vector<int64_t>& getStack();

 private:
  std::unordered_map<std::string, Value> storage;
  std::vector<int64_t> stack;
  std::stack<std::unordered_map<std::string, Value>> current_name_scope;
  size_t operationCount;
  GarbageCollector gc;

  void add();
  void subtract();
  void multiply();
  void divide();
  void modulo();
  void performArithmeticOperation(const std::function<int64_t(int64_t, int64_t)>& op);

  void equals();
  void lessThan();
  void greaterThan();
  void lessThanOrEqual();
  void greaterThanOrEqual();
  void performComparisonOperation(const std::function<bool(int64_t, int64_t)>& op);

  void declareVar(const std::vector<int64_t>& operands);
  void assignVar(const std::vector<int64_t>& operands);
  void loadVar(const std::vector<int64_t>& operands);

  void declareArray(const std::vector<int64_t>& operands);
  void assignArrayElement(const std::vector<int64_t>& operands);
  void loadArrayElement(const std::vector<int64_t>& operands);

  void loadConst(const std::vector<int64_t>& operands);

  void print();
};

#endif // VIRTUAL_MACHINE_H