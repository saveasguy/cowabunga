#include "ast_node.h"

#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include "driver.h"

namespace kaleidoc {

// Integral number AST node

IntegralNumberAstNode::IntegralNumberAstNode(int value) : value_{value} {}

std::unique_ptr<AstNode> IntegralNumberAstNode::Clone() const {
  return std::make_unique<IntegralNumberAstNode>(*this);
}

// Variable AST node

VariableAstNode::VariableAstNode(std::string name) : name_{std::move(name)} {}

std::unique_ptr<AstNode> VariableAstNode::Clone() const {
  return std::make_unique<VariableAstNode>(*this);
};

// Binary expression AST node

BinaryExpressionAstNode::BinaryExpressionAstNode(OperatorId op,
                                                 std::unique_ptr<AstNode> lhs,
                                                 std::unique_ptr<AstNode> rhs)
    : op_{op}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {}

BinaryExpressionAstNode::BinaryExpressionAstNode(
    const BinaryExpressionAstNode &rhs)
    : op_{rhs.op_}, lhs_{rhs.lhs_->Clone()}, rhs_{rhs.rhs_->Clone()} {}

BinaryExpressionAstNode &BinaryExpressionAstNode::operator=(
    const BinaryExpressionAstNode &rhs) {
  BinaryExpressionAstNode new_node{rhs};
  std::swap(*this, new_node);
  return *this;
}

std::unique_ptr<AstNode> BinaryExpressionAstNode::Clone() const {
  return std::make_unique<BinaryExpressionAstNode>(*this);
}

// Call expression AST node

CallExpressionAstNode::CallExpressionAstNode(
    std::string callee, std::vector<std::unique_ptr<AstNode>> args)
    : callee_{std::move(callee)}, args_{std::move(args)} {}

CallExpressionAstNode::CallExpressionAstNode(const CallExpressionAstNode &rhs)
    : callee_{rhs.callee_} {
  std::vector<std::unique_ptr<AstNode>> args;
  args.reserve(rhs.args_.size());
  for (const auto &arg : rhs.args_) { args.push_back(arg->Clone()); }
  args_ = std::move(args);
}

CallExpressionAstNode &CallExpressionAstNode::operator=(
    const CallExpressionAstNode &rhs) {
  CallExpressionAstNode new_node{rhs};
  std::swap(*this, new_node);
  return *this;
}

std::unique_ptr<AstNode> CallExpressionAstNode::Clone() const {
  return std::make_unique<CallExpressionAstNode>(*this);
}

// Prototype AST node

PrototypeAstNode::PrototypeAstNode(std::string function_name,
                                   std::vector<std::string> args)
    : name_{std::move(function_name)}, args_{std::move(args)} {}

std::unique_ptr<AstNode> PrototypeAstNode::Clone() const {
  return std::make_unique<PrototypeAstNode>(*this);
}

// Function AST node

FunctionAstNode::FunctionAstNode(std::unique_ptr<AstNode> proto,
                                 std::unique_ptr<AstNode> body)
    : proto_{std::move(proto)}, body_{std::move(body)} {}

FunctionAstNode::FunctionAstNode(const FunctionAstNode &rhs)
    : proto_{rhs.proto_->Clone()}, body_{rhs.body_->Clone()} {}

FunctionAstNode &FunctionAstNode::operator=(const FunctionAstNode &rhs) {
  FunctionAstNode new_node{rhs};
  std::swap(*this, new_node);
  return *this;
}

std::unique_ptr<AstNode> FunctionAstNode::Clone() const
{
  return std::make_unique<FunctionAstNode>(*this);
}

}  // namespace kaleidoc
