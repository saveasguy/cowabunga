#include "cbc/ASTNodes.h"
#include "cbc/ASTPasses.h"
#include "cbc/Driver.h"
#include "common/Metadata.h"
#include "common/Token.h"

#include <cassert>
#include <string>
#include <utility>

namespace cb {

VariableASTNode::VariableASTNode(const common::Token &Tok)
    : MetadataStorage(Tok.MetadataStorage),
      Name(Tok.MetadataStorage.get(common::Stringified)) {
  assert(Tok.id() == Identifier && "Expected identifier token");
}

void VariableASTNode::acceptASTPass(IASTPass &Pass) { Pass.accept(*this); }

void VariableASTNode::print(std::ostream &Out) const {
  Out << "Variable '" << Name << "'";
}

IntegralNumberASTNode::IntegralNumberASTNode(const common::Token &Tok)
    : MetadataStorage(Tok.MetadataStorage),
      Value(std::stoll(Tok.MetadataStorage.get(common::Stringified))) {
  assert(Tok.id() == IntegralNumber && "Expected integral number token");
}

void IntegralNumberASTNode::acceptASTPass(IASTPass &Pass) {
  Pass.accept(*this);
}

void IntegralNumberASTNode::print(std::ostream &Out) const {
  Out << "Integral Number '" << Value << "'";
}

BinaryExpressionASTNode::BinaryExpressionASTNode(
    const common::Token &Tok, std::unique_ptr<IASTNode> LHSExpression,
    std::unique_ptr<IASTNode> RHSExpression)
    : MetadataStorage(Tok.MetadataStorage), LHS(std::move(LHSExpression)),
      RHS(std::move(RHSExpression)), OpID(static_cast<OperatorID>(Tok.id())) {
  assert(Tok.id() > BinaryOperatorsRangeBegin &&
         Tok.id() < BinaryOperatorsRangeEnd &&
         "Expected binary expression token");
}

BinaryExpressionASTNode::BinaryExpressionASTNode(
    const BinaryExpressionASTNode &RHSBinaryExpression)
    : MetadataStorage(RHSBinaryExpression.MetadataStorage),
      LHS(RHSBinaryExpression.LHS->clone()),
      RHS(RHSBinaryExpression.RHS->clone()) {}

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
  Out << "Binary Expression '" << MetadataStorage.get(common::Stringified)
      << "'";
}

ParenthesizedExpressionASTNode::ParenthesizedExpressionASTNode(
    const common::Token &OpenParentheses,
    std::unique_ptr<IASTNode> InternalExpression,
    const common::Token &CloseParentheses)
    : Expression(std::move(InternalExpression)) {
  MetadataStorage.set(
      common::Stringified,
      OpenParentheses.MetadataStorage.get(common::Stringified) +
          CloseParentheses.MetadataStorage.get(common::Stringified));
}

ParenthesizedExpressionASTNode::ParenthesizedExpressionASTNode(
    const ParenthesizedExpressionASTNode &RHS)
    : MetadataStorage(RHS.MetadataStorage),
      Expression(RHS.Expression->clone()) {}

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
  Out << "Parenthesized Expression '" << MetadataStorage.get(common::Stringified) << "'";
}

CompoundExpressionASTNode::CompoundExpressionASTNode(
    const common::Token &Tok, std::unique_ptr<IASTNode> FirstExpression,
    std::unique_ptr<IASTNode> SecondExpression)
    : MetadataStorage(Tok.MetadataStorage), First(std::move(FirstExpression)),
      Second(std::move(SecondExpression)) {
  assert(Tok.id() == ExpressionSeparator &&
         "Expected expression separator token");
}

CompoundExpressionASTNode::CompoundExpressionASTNode(
    const CompoundExpressionASTNode &RHS)
    : MetadataStorage(RHS.MetadataStorage), First(RHS.First->clone()),
      Second(RHS.Second->clone()) {}

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
  Out << "Expression Sequence '" << MetadataStorage.get(common::Stringified)
      << "'";
}

} // namespace cb
