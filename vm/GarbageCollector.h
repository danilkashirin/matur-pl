#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <chrono>
#include <stack>

using Value = std::variant<int64_t, std::vector<int64_t>>;

class GarbageCollector {
 public:
  GarbageCollector(std::unordered_map<std::string, Value>& storage,
                   std::vector<int64_t>& stack,
                   std::stack<std::unordered_map<std::string, Value>>& scopes);

  void collect();
  void cleanup();

 private:
  std::unordered_map<std::string, Value>& storage;
  std::vector<int64_t>& stack;
  std::stack<std::unordered_map<std::string, Value>>& scopes;

  void mark(std::unordered_set<std::string>& marked);
  void sweep(const std::unordered_set<std::string>& marked, std::vector<std::string>& removedItems);
};

#endif // GARBAGE_COLLECTOR_H