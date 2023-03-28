#include "cowabunga/CBC/ASTNodes.h"

#include "cowabunga/CBC/ASTPasses.h"
#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Lexer/Token.h"

#include <cassert>
#include <memory>
#include <string>
#include <utility>

using namespace cb;

IASTNode::~IASTNode() {}

VariableASTNode::VariableASTNode(const Token &Tok) : Name(Tok.getLexeme()) {
  assert(Tok.getID() == TID_Identifier && "Expected identifier token");
}

void VariableASTNode::acceptASTPass(IASTPass &Pass) { Pass.accept(*this); }

void VariableASTNode::print(std::ostream &Out) const {
  Out << "Variable '" << Name << "'";
}

IntegralNumberASTNode::IntegralNumberASTNode(const Token &Tok)
    : Value(Tok.getLexeme()) {
  assert(Tok.getID() == TID_IntegralNumber && "Expected integral number token");
}

void IntegralNumberASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void IntegralNumberASTNode::print(std::ostream &Out) const {
  Out << "Integral Number '" << Value << "'";
}

AssignmentExpressionASTNode::AssignmentExpressionASTNode(
    std::string Assignment, std::unique_ptr<IASTNode> LHSNode,
    std::unique_ptr<IASTNode> RHSNode)
    : LHS(std::move(LHSNode)), RHS(std::move(RHSNode)),
      AssignmentLexeme(std::move(Assignment)) {}

AssignmentExpressionASTNode::AssignmentExpressionASTNode(
    const AssignmentExpressionASTNode &RHSNode)
    : LHS(RHSNode.LHS->clone()), RHS(RHSNode.RHS->clone()),
      AssignmentLexeme(RHSNode.AssignmentLexeme) {}

AssignmentExpressionASTNode &
AssignmentExpressionASTNode::operator=(const AssignmentExpressionASTNode &RHS) {
  AssignmentExpressionASTNode NewASTNode(RHS);
  std::swap(*this, NewASTNode);
  return *this;
}

void AssignmentExpressionASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void AssignmentExpressionASTNode::print(std::ostream &Out) const {
  Out << "Assignment Expression '" << AssignmentLexeme << "'";
}

CompoundExpressionASTNode::CompoundExpressionASTNode(
    std::string ExpressionSeparator,
    std::vector<std::unique_ptr<IASTNode>> ExpressionList)
    : Expressions(std::move(ExpressionList)),
      ExpressionSeparatorLexeme(std::move(ExpressionSeparator)) {}

CompoundExpressionASTNode::CompoundExpressionASTNode(
    const CompoundExpressionASTNode &RHS)
    : ExpressionSeparatorLexeme(RHS.ExpressionSeparatorLexeme) {
  Expressions.reserve(RHS.Expressions.size());
  for (auto &Expression : RHS.Expressions) {
    Expressions.push_back(Expression->clone());
  }
}

CompoundExpressionASTNode &
CompoundExpressionASTNode::operator=(const CompoundExpressionASTNode &RHS) {
  CompoundExpressionASTNode NewASTNode(RHS);
  std::swap(*this, NewASTNode);
  return *this;
}

void CompoundExpressionASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void CompoundExpressionASTNode::print(std::ostream &Out) const {
  Out << "Expression Sequence '" << ExpressionSeparatorLexeme << "'";
}

CallExpressionASTNode::CallExpressionASTNode(
    const Token &Tok,
    std::vector<std::unique_ptr<IASTNode>> ParamsList)
    : Parameters(std::move(ParamsList)),
      FuncName(Tok.getLexeme()) {}

CallExpressionASTNode::CallExpressionASTNode(
    const CallExpressionASTNode &RHS)
    : FuncName(RHS.FuncName) {
  Parameters.reserve(RHS.Parameters.size());
  for (auto &Param : RHS.Parameters) {
    Parameters.push_back(Param->clone());
  }
}

CallExpressionASTNode &
CallExpressionASTNode::operator=(const CallExpressionASTNode &RHS) {
  CallExpressionASTNode NewASTNode(RHS);
  std::swap(*this, NewASTNode);
  return *this;
}

void CallExpressionASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void CallExpressionASTNode::print(std::ostream &Out) const {
  Out << "Call Expression '" << FuncName << "'";
}
