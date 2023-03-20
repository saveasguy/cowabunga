#include "cowabunga/CBC/ASTNodes.h"

#include "cowabunga/CBC/ASTPasses.h"
#include "cowabunga/CBC/Definitions.h"
#include "cowabunga/Lexer/Token.h"

#include <cassert>
#include <memory>
#include <string>
#include <utility>

using namespace cb;

IASTNode::~IASTNode() {}

VariableASTNode::VariableASTNode(const Token &Tok) : Name(Tok.lexeme()) {
  assert(Tok.id() == Identifier && "Expected identifier token");
}

void VariableASTNode::acceptASTPass(IASTPass &Pass) { Pass.accept(*this); }

void VariableASTNode::print(std::ostream &Out) const {
  Out << "Variable '" << Name << "'";
}

IntegralNumberASTNode::IntegralNumberASTNode(const Token &Tok)
    : Value(Tok.lexeme()) {
  assert(Tok.id() == IntegralNumber && "Expected integral number token");
}

void IntegralNumberASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void IntegralNumberASTNode::print(std::ostream &Out) const {
  Out << "Integral Number '" << Value << "'";
}

BinaryExpressionASTNode::BinaryExpressionASTNode(
    const Token &Tok, std::unique_ptr<IASTNode> LHSExpression,
    std::unique_ptr<IASTNode> RHSExpression)
    : LHS(std::move(LHSExpression)), RHS(std::move(RHSExpression)),
      OperatorLexeme(Tok.lexeme()), OpID(static_cast<OperatorID>(Tok.id())) {
  assert(Tok.id() > BinaryOperatorsRangeBegin &&
         Tok.id() < BinaryOperatorsRangeEnd &&
         "Expected binary expression token");
}

BinaryExpressionASTNode::BinaryExpressionASTNode(
    const BinaryExpressionASTNode &RHSBinaryExpression)
    : LHS(RHSBinaryExpression.LHS->clone()),
      RHS(RHSBinaryExpression.RHS->clone()),
      OperatorLexeme(RHSBinaryExpression.OperatorLexeme),
      OpID(RHSBinaryExpression.OpID) {}

BinaryExpressionASTNode &BinaryExpressionASTNode::operator=(
    const BinaryExpressionASTNode &RHSBinaryExpression) {
  BinaryExpressionASTNode NewASTNode(RHSBinaryExpression);
  std::swap(*this, NewASTNode);
  return *this;
}

void BinaryExpressionASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void BinaryExpressionASTNode::print(std::ostream &Out) const {
  Out << "Binary Expression '" << OperatorLexeme << "'";
}

ParenthesizedExpressionASTNode::ParenthesizedExpressionASTNode(
    const Token &OpenParentheses, std::unique_ptr<IASTNode> InternalExpression,
    const Token &CloseParentheses)
    : Expression(std::move(InternalExpression)),
      OpenParenthesesLexeme(OpenParentheses.lexeme()),
      CloseParenthesesLexeme(CloseParentheses.lexeme()) {}

ParenthesizedExpressionASTNode::ParenthesizedExpressionASTNode(
    const ParenthesizedExpressionASTNode &RHS)
    : Expression(RHS.Expression->clone()),
      OpenParenthesesLexeme(RHS.OpenParenthesesLexeme),
      CloseParenthesesLexeme(RHS.CloseParenthesesLexeme) {}

ParenthesizedExpressionASTNode &ParenthesizedExpressionASTNode::operator=(
    const ParenthesizedExpressionASTNode &RHS) {
  ParenthesizedExpressionASTNode NewAST(RHS);
  std::swap(*this, NewAST);
  return *this;
}

void ParenthesizedExpressionASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void ParenthesizedExpressionASTNode::print(std::ostream &Out) const {
  Out << "Parenthesized Expression '" << OpenParenthesesLexeme << "..."
      << CloseParenthesesLexeme << "'";
}

CompoundExpressionASTNode::CompoundExpressionASTNode(
    const Token &Tok, std::unique_ptr<IASTNode> FirstExpression,
    std::unique_ptr<IASTNode> SecondExpression)
    : First(std::move(FirstExpression)), Second(std::move(SecondExpression)),
      ExpressionSeparatorLexeme(Tok.lexeme()) {
  assert(Tok.id() == ExpressionSeparator &&
         "Expected expression separator token");
}

CompoundExpressionASTNode::CompoundExpressionASTNode(
    const CompoundExpressionASTNode &RHS)
    : First(RHS.First->clone()), Second(RHS.Second->clone()),
      ExpressionSeparatorLexeme(RHS.ExpressionSeparatorLexeme) {}

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
