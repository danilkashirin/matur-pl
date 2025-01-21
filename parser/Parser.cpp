#include "Parser.h"

#include <random>

#include "VariableAST.h"
#include "PrintAST.h"
#include "NumberAST.h"
#include "ArithmeticOpNode.h"
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "AssigmentAST.h"
#include "ArrayAST.h"
#include "BooleanAST.h"
#include "CompareOpNode.h"
#include "ForNode.h"
#include "FunctionAST.h"
#include "IfNode.h"

Parser::Parser(const std::string& input)
    : lexer(input), currentToken(lexer.getNextToken()) {}

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
  std::vector<std::unique_ptr<ASTNode>> nodes;

  while (currentToken.type != TokenType::Jawohl) {
    if (currentToken.type == TokenType::Type) {
      nodes.push_back(std::unique_ptr<ASTNode>(parseVariableDeclaration()));
    } else if (currentToken.type == TokenType::Identifier) {
      nodes.push_back(std::unique_ptr<ASTNode>(parseAssignment()));
    } else if (currentToken.type == TokenType::Print) {
      nodes.push_back(std::unique_ptr<ASTNode>(parsePrintStatement()));
    } else if (currentToken.type == TokenType::Array) {
      nodes.push_back(std::unique_ptr<ASTNode>(parseArrayDeclaration()));
    } else if (currentToken.type == TokenType::For) {
      nodes.push_back(std::unique_ptr<ASTNode>(parseFor()));
    } else if (currentToken.type == TokenType::If) {
      nodes.push_back(std::unique_ptr<ASTNode>(parseIf()));
    } else if (currentToken.type == TokenType::Def) {
      nodes.push_back(std::unique_ptr<ASTNode>(parseFunction()));
    } else if (currentToken.type == TokenType::EndOfFile) {
      while (true) {
        std::cout << "!! ACHTUNG ALARMA !!";
      }
    }
    else {
      throw std::runtime_error("Unexpected token during parsing");
    }
  }

  return nodes;
}

void Parser::consumeToken() {
  currentToken = lexer.getNextToken();
}

void Parser::expect(TokenType expectedType) {
  if (currentToken.type != expectedType) {
    throw std::runtime_error("Expected a different token type");
  }
  consumeToken();
}

ASTNode* Parser::parseVariableDeclaration() {
  std::string type = currentToken.value;
  consumeToken();

  if (currentToken.type != TokenType::Identifier) {
    throw std::runtime_error("Expected variable name after type");
  }

  std::string name = currentToken.value;
  consumeToken();

  expect(TokenType::Equals);
  auto expression = std::unique_ptr<ASTNode>(parseExpression());
  expect(TokenType::Semicolon);

  return new VariableDeclAST(type, name, std::move(expression));
}

ASTNode* Parser::parseAssignment() {
  std::string varName = currentToken.value;
  consumeToken();
  if (currentToken.type == TokenType::LParen) {
    return parseCallFunction(varName);
  }

  std::unique_ptr<ASTNode> leftSide;
  if (currentToken.type == TokenType::LBracket) {
    leftSide = std::unique_ptr<ASTNode>(parseArrayAccess(varName));
  } else {
    leftSide = std::make_unique<VariableRefAST>(varName);
  }

  expect(TokenType::Equals);

  auto expression = std::unique_ptr<ASTNode>(parseExpression());
  expect(TokenType::Semicolon);

  return new AssignmentAST(std::move(leftSide), std::move(expression));
}

ASTNode* Parser::parsePrintStatement() {
  consumeToken();
  expect(TokenType::LParen);
  auto expression = std::unique_ptr<ASTNode>(parseExpression());
  expect(TokenType::RParen);
  expect(TokenType::Semicolon);

  return new PrintAST(std::move(expression));
}

ASTNode* Parser::parseExpression() {
  auto left = parseTerm();

  while (currentToken.type == TokenType::LessThan || currentToken.type == TokenType::GreaterThan ||
      currentToken.type == TokenType::LessThanOrEqual || currentToken.type == TokenType::GreaterThanOrEqual ||
      currentToken.type == TokenType::EqualsEquals) {
    CompareOpNode::Operator compareOp;
    switch (currentToken.type) {
      case TokenType::LessThan:
        compareOp = CompareOpNode::Operator::LESS_THAN;
        break;
      case TokenType::GreaterThan:
        compareOp = CompareOpNode::Operator::GREATER_THAN;
        break;
      case TokenType::LessThanOrEqual:
        compareOp = CompareOpNode::Operator::LESS_THAN_OR_EQUAL;
        break;
      case TokenType::GreaterThanOrEqual:
        compareOp = CompareOpNode::Operator::GREATER_THAN_OR_EQUAL;
        break;
      case TokenType::EqualsEquals:
        compareOp = CompareOpNode::Operator::EQUALS;
        break;
      default:
        throw std::runtime_error("Unexpected comparison operator");
    }
    consumeToken();
    auto right = parseTerm();
    left = new CompareOpNode(left, compareOp, right);
  }

  while (currentToken.type == TokenType::Plus || currentToken.type == TokenType::Minus) {
    auto opType = currentToken.type;
    consumeToken();
    auto right = parseTerm();
    auto op = (opType == TokenType::Plus) ? ArithmeticOpNode::Operator::ADD : ArithmeticOpNode::Operator::SUBTRACT;
    left = new ArithmeticOpNode(left, op, right);
  }

  return left;
}

ASTNode* Parser::parseTerm() {
  auto left = parseFactor();

  while (currentToken.type == TokenType::Asterisk || currentToken.type == TokenType::Slash || currentToken.type == TokenType::Modulo) {
    auto opType = currentToken.type;
    consumeToken();
    auto right = parseFactor();
    auto op = (opType == TokenType::Asterisk) ? ArithmeticOpNode::Operator::MULTIPLY :
              (opType == TokenType::Slash) ? ArithmeticOpNode::Operator::DIVIDE :
              ArithmeticOpNode::Operator::MODULO;
    left = new ArithmeticOpNode(left, op, right);
  }

  return left;
}

ASTNode* Parser::parseFactor() {
  if (currentToken.type == TokenType::Number || currentToken.type == TokenType::Minus) {
    bool minus = false;
    if (currentToken.type == TokenType::Minus) {
      minus = true;
      consumeToken();
    }
    int64_t value = minus ? -1 * std::stoll(currentToken.value) : std::stoll(currentToken.value);
    consumeToken();
    return new NumberAST(value);
  } else if (currentToken.type == TokenType::Boolean) {
    bool value = (currentToken.value == "true");
    consumeToken();
    return new BooleanAST(value);
  } else if (currentToken.type == TokenType::Identifier) {
    std::string varName = currentToken.value;
    consumeToken();
    if (currentToken.type == TokenType::LBracket) {
      auto node = parseArrayAccess(varName);
      return node;
    }if (currentToken.type == TokenType::LParen) {
      return parseCallFunction(varName);
    }
    return new VariableRefAST(varName);
  } else if (currentToken.type == TokenType::LParen) {
    consumeToken();
    auto node = parseExpression();
    expect(TokenType::RParen);
    return node;
  } else if (currentToken.type == TokenType::Array) {
    return parseArrayDeclaration();
  } else if (currentToken.type == TokenType::Type) {
    return parseVariableDeclaration();
  } else if (currentToken.type == TokenType::Print) {
    return parsePrintStatement();
  } else if (currentToken.type == TokenType::For) {
    return parseFor();
  } else if (currentToken.type == TokenType::If) {
    return parseIf();
  } else if (currentToken.type == TokenType::Return) {
    return parseReturn();
  } else {
    throw std::runtime_error("Unexpected factor");
  }
}

std::vector<int64_t> generateRandomVector(std::size_t size) {
  std::random_device rd;
  std::mt19937_64 eng(rd());
  std::uniform_int_distribution<int64_t> distr(0, 10000);

  std::vector<int64_t> randomNumbers;
  randomNumbers.reserve(size);

  for (std::size_t i = 0; i < size; ++i) {
    randomNumbers.push_back(distr(eng));
  }

  return randomNumbers;
}

ASTNode* Parser::parseArrayDeclaration() {
  consumeToken();
  expect(TokenType::LessThan);

  std::string elementType = currentToken.value;
  consumeToken();

  expect(TokenType::GreaterThan);
  std::string arrayName = currentToken.value;
  consumeToken();

  int64_t size = 0;
  if (currentToken.type == TokenType::LParen) {
    consumeToken();
    size = parseSize();
    expect(TokenType::RParen);
  }

  if (currentToken.type == TokenType::Equals) {
    consumeToken();
    if (currentToken.type == TokenType::Random) {
      expect(TokenType::Random);
      expect(TokenType::LParen);
      int size_r = parseValue(elementType);
      expect(TokenType::RParen);
      expect(TokenType::Semicolon);
      return new ArrayDeclAST(elementType, arrayName, size_r, generateRandomVector(size_r));
    }
    auto elements = parseArrayElements(elementType);
    expect(TokenType::Semicolon);
    if (size > elements.size()) {
      for (int i = 0; size - elements.size(); i++) {
        elements.push_back(0);
      }
    }
    return new ArrayDeclAST(elementType, arrayName, elements.size(), elements);
  }

  expect(TokenType::Semicolon);
  std::vector<int64_t> elements_empty_array;
  elements_empty_array.reserve(size);
  for (int i = 0; i < size; ++i) {
    elements_empty_array.push_back(0);
  }

  return new ArrayDeclAST(elementType, arrayName, size, elements_empty_array);
}

int64_t Parser::parseValue(const std::string& type) {
  if (type == "bool") {
    std::string value = currentToken.value;
    consumeToken();
    return value == "true" ? 1 : 0;
  } else if (currentToken.type == TokenType::Number) {
    int64_t numValue = std::stoll(currentToken.value);
    consumeToken();
    return numValue;
  } else {
    throw std::runtime_error("Unexpected value type");
  }
}

std::vector<int64_t> Parser::parseArrayElements(const std::string& elementType) {
  expect(TokenType::LBracket);
  std::vector<int64_t> elements;

  while (currentToken.type != TokenType::RBracket) {
    elements.push_back(parseValue(elementType));

    if (currentToken.type == TokenType::RBracket) {
      break;
    }

    expect(TokenType::Comma);
  }

  consumeToken();
  return elements;
}

int64_t Parser::parseSize() {
  if (currentToken.type != TokenType::Number) {
    throw std::runtime_error("Expected array size as a number");
  }
  int64_t size = std::stoll(currentToken.value);
  consumeToken();
  return size;
}

ASTNode* Parser::parseArrayAccess(const std::string& arrayName) {
  expect(TokenType::LBracket);
  auto* index = parseAccessValueForArray();
  expect(TokenType::RBracket);
  return new ArrayAccessAST(arrayName, index);
}

ASTNode* Parser::parseAccessValueForArray() {
  if (currentToken.type == TokenType::Boolean) {
    std::string value = currentToken.value;
    consumeToken();
    return new BooleanAST(value == "true");
  } else if (currentToken.type == TokenType::Number || currentToken.type == TokenType::Minus) {
    bool minus = false;
    if (currentToken.type == TokenType::Minus) {
      minus = true;
      consumeToken();
      if (currentToken.type != TokenType::Number) {
        throw std::runtime_error("Unexpected value type");
      }
    }
    int64_t numValue = std::stoll(currentToken.value);
    consumeToken();
    if (minus && numValue == 0) {
      throw std::runtime_error("Index out of bounds");
    }
    return new NumberAST(minus ? -numValue : numValue);
  } else if (currentToken.type == TokenType::Identifier) {
    return parseExpression();
  } else if (currentToken.type == TokenType::Return) {
    return parseReturn();
  } else {
    return parseExpression();
  }
}

ASTNode* Parser::parseFor() {
  consumeToken();
  std::string iterator_name = currentToken.value;
  consumeToken();
  expect(TokenType::In);
  expect(TokenType::LessThan);

  ASTNode* start = nullptr;
  ASTNode* finish = nullptr;
  ASTNode* step = nullptr;

  finish = parseFactor();

  if (currentToken.type == TokenType::Comma) {
    start = finish;
    consumeToken();
    finish = parseFactor();
  } else {
    start = new NumberAST(1);
  }

  if (currentToken.type == TokenType::Comma) {
    consumeToken();
    step = parseFactor();
  } else {
    step = new NumberAST(1);
  }

  expect(TokenType::GreaterThan);
  expect(TokenType::CurlyLBracket);

  std::vector<std::unique_ptr<ASTNode>> elements;
  while (currentToken.type != TokenType::CurlyRBracket) {
    ASTNode* rawPointer = nullptr;
    if (currentToken.type == TokenType::Identifier) {
      rawPointer = parseAssignment();
    } else {
      rawPointer = parseFactor();
    }
    auto element = std::unique_ptr<ASTNode>(rawPointer);
    elements.push_back(std::move(element));
  }

  expect(TokenType::CurlyRBracket);
  expect(TokenType::Semicolon);

  return new ForNode(iterator_name, start, finish, step, std::move(elements));
}

ASTNode* Parser::parseIf() {
  consumeToken();
  expect(TokenType::LParen);
  ASTNode* condition = parseExpression();
  expect(TokenType::RParen);
  expect(TokenType::CurlyLBracket);
  std::vector<std::unique_ptr<ASTNode>> elements;
  while (currentToken.type != TokenType::CurlyRBracket) {
    if (currentToken.type == TokenType::Identifier) {
      ASTNode* rawPointer = parseAssignment();
      auto element = std::unique_ptr<ASTNode>(rawPointer);
      elements.push_back(std::move(element));
    } else {
      ASTNode* rawPointer = parseFactor();
      auto element = std::unique_ptr<ASTNode>(rawPointer);
      elements.push_back(std::move(element));
    }
  }
  expect(TokenType::CurlyRBracket);
  std::vector<std::unique_ptr<ASTNode>> else_elements;
  if (currentToken.type != TokenType::Semicolon) {
    expect(TokenType::Else);
    expect(TokenType::CurlyLBracket);
    while (currentToken.type != TokenType::CurlyRBracket) {
      if (currentToken.type == TokenType::Identifier) {
        ASTNode* rawPointer = parseAssignment();
        auto element = std::unique_ptr<ASTNode>(rawPointer);
        else_elements.push_back(std::move(element));
      } else {
        ASTNode* rawPointer = parseFactor();
        auto element = std::unique_ptr<ASTNode>(rawPointer);
        else_elements.push_back(std::move(element));
      }
    }
    expect(TokenType::CurlyRBracket);
  }
  expect(TokenType::Semicolon);
  return new IfNode(condition, std::move(elements), std::move(else_elements));
}


ASTNode* Parser::parseFunction() {
  consumeToken();
  std::string functionName = currentToken.value;
  consumeToken();
  expect(TokenType::LParen);
  auto arguments = parsesFunctionArguments();
  expect(TokenType::CurlyLBracket);
  std::vector<std::unique_ptr<ASTNode>> bodyElements;

  while (currentToken.type != TokenType::CurlyRBracket) {
    if (currentToken.type == TokenType::Identifier) {
      ASTNode* rawPointer = parseAssignment();
      auto element = std::unique_ptr<ASTNode>(rawPointer);
      bodyElements.push_back(std::move(element));
    } else {
      ASTNode* rawPointer = parseFactor();
      auto element = std::unique_ptr<ASTNode>(rawPointer);
      bodyElements.push_back(std::move(element));
    }
  }

  expect(TokenType::CurlyRBracket);
  expect(TokenType::Semicolon);

  return new FunctionDeclNode(functionName, std::move(arguments), std::move(bodyElements));
};

std::vector<std::string> Parser::parsesFunctionArguments() {
  std::vector<std::string> arguments;
  while (currentToken.type != TokenType::RParen) {
    std::string name = currentToken.value;
    consumeToken();
    if (currentToken.type == TokenType::Comma) {
      expect(TokenType::Comma);
    }
    arguments.push_back(name);
  }
  expect(TokenType::RParen);
  return arguments;
}

ASTNode* Parser::parseReturn() {
  consumeToken();
  auto rawPointer = parseExpression();
  expect(TokenType::Semicolon);
  return new ReturnNode(rawPointer);
}

ASTNode* Parser::parseCallFunction(std::string name) {
  expect(TokenType::LParen);
  std::vector<std::unique_ptr<ASTNode>> arguments;
  while (currentToken.type != TokenType::RParen) {
    ASTNode* rawPointer = parseExpression();
    auto element = std::unique_ptr<ASTNode>(rawPointer);
    arguments.push_back(std::move(element));
    if (currentToken.type == TokenType::Comma) {
      expect(TokenType::Comma);
    }
  }
  expect(TokenType::RParen);
  return new FunctionCallNode(std::move(name), std::move(arguments));
}