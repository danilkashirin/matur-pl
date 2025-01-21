#include "GarbageCollector.h"
#include <sstream>

GarbageCollector::GarbageCollector(std::unordered_map<std::string, Value>& storage,
                                   std::vector<int64_t>& stack,
                                   std::stack<std::unordered_map<std::string, Value>>& scopes)
    : storage(storage), stack(stack), scopes(scopes) {}

void GarbageCollector::collect() {
  std::unordered_set<std::string> marked;
  mark(marked);

  std::vector<std::string> removedItems;
  sweep(marked, removedItems);
}

void GarbageCollector::cleanup() {
  storage.clear();
  stack.clear();
  while (!scopes.empty()) {
    scopes.pop();
  }
}

void GarbageCollector::mark(std::unordered_set<std::string>& marked) {
  for (const auto& value : stack) {
    marked.insert(std::to_string(value));
  }

  for (const auto& [key, val] : storage) {
    marked.insert(key);
  }

  std::stack<std::unordered_map<std::string, Value>> tempScopes = scopes;
  while (!tempScopes.empty()) {
    const auto& scope = tempScopes.top();
    for (const auto& [key, val] : scope) {
      marked.insert(key);
    }
    tempScopes.pop();
  }
}

void GarbageCollector::sweep(const std::unordered_set<std::string>& marked, std::vector<std::string>& removedItems) {
  for (auto it = storage.begin(); it != storage.end();) {
    if (marked.find(it->first) == marked.end()) {
      removedItems.push_back(it->first);
      it = storage.erase(it);
    } else {
      ++it;
    }
  }
}