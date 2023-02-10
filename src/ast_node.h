#ifndef KALEIDOC_SRC_AST_NODE_H_
#define KALEIDOC_SRC_AST_NODE_H_

#include <memory>
#include <string>
#include <vector>

#include "driver.h"

namespace kaleidoc {

class IntegralNumberAstNode : public AstNode {
 public:
  IntegralNumberAstNode(int value);

  std::unique_ptr<AstNode> Clone() const override;

 private:
  int value_;
};

class VariableAstNode : public AstNode {
 public:
  VariableAstNode(std::string name);

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::string name_;
};

class BinaryExpressionAstNode : public AstNode {
 public:
  BinaryExpressionAstNode(OperatorId op, std::unique_ptr<AstNode> lhs,
                          std::unique_ptr<AstNode> rhs);

  BinaryExpressionAstNode(const BinaryExpressionAstNode &rhs);

  BinaryExpressionAstNode(BinaryExpressionAstNode &&rhs) noexcept = default;

  BinaryExpressionAstNode &operator=(const BinaryExpressionAstNode &rhs);

  BinaryExpressionAstNode &operator=(BinaryExpressionAstNode &&rhs) noexcept =
      default;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  OperatorId op_;
  std::unique_ptr<AstNode> lhs_;
  std::unique_ptr<AstNode> rhs_;
};

class CallExpressionAstNode : public AstNode {
 public:
  CallExpressionAstNode(std::string callee,
                        std::vector<std::unique_ptr<AstNode>> args);

  CallExpressionAstNode(const CallExpressionAstNode &rhs);

  CallExpressionAstNode(CallExpressionAstNode &&rhs) noexcept = default;

  CallExpressionAstNode &operator=(const CallExpressionAstNode &rhs);

  CallExpressionAstNode &operator=(CallExpressionAstNode &&rhs) noexcept =
      default;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::string callee_;
  std::vector<std::unique_ptr<AstNode>> args_;
};

class PrototypeAstNode : public AstNode {
 public:
  PrototypeAstNode(std::string function_name, std::vector<std::string> args);

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::string name_;
  std::vector<std::string> args_;
};

class FunctionAstNode : public AstNode {
 public:
  FunctionAstNode(std::unique_ptr<AstNode> proto,
                  std::unique_ptr<AstNode> body);

  FunctionAstNode(const FunctionAstNode &rhs);

  FunctionAstNode(FunctionAstNode &&rhs) noexcept = default;

  FunctionAstNode &operator=(const FunctionAstNode &rhs);

  FunctionAstNode &operator=(FunctionAstNode &&rhs) noexcept = default;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::unique_ptr<AstNode> proto_;
  std::unique_ptr<AstNode> body_;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_AST_NODE_H_
