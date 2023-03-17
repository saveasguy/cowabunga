#include "cbc/ASTNodes.h"
#include "cbc/Driver.h"
#include "common/Metadata.h"
#include "common/Token.h"

#include <cassert>
#include <string>
#include <utility>

namespace cb {

VariableASTNode::VariableASTNode(const common::Token &Tok)
    : MetadataStorage(Tok.metadata()), Name(Tok.metadata(common::Stringified)) {
  assert(Tok.id() == Identifier && "Expected identifier token");
}

void VariableASTNode::acceptASTPass(IASTPass &Pass) {}

void VariableASTNode::print(std::ostream &Out) const {
  Out << "Variable '" << name() << "'";
}

std::string VariableASTNode::name() const { return Name; }

IntegralNumberASTNode::IntegralNumberASTNode(const common::Token &Tok)
    : MetadataStorage(Tok.metadata()),
      Value(std::stoll(Tok.metadata(common::Stringified))) {
  assert(Tok.id() == IntegralNumber && "Expected integral number token");
}

void IntegralNumberASTNode::acceptASTPass(IASTPass &Pass) {}

void IntegralNumberASTNode::print(std::ostream &Out) const {
  Out << "Integral Number '" << value() << "'";
}

long long IntegralNumberASTNode::value() const noexcept { return Value; }

BinaryExpressionASTNode::BinaryExpressionASTNode(
    const common::Token &Tok, std::unique_ptr<IASTNode> LHSExpression,
    std::unique_ptr<IASTNode> RHSExpression)
    : MetadataStorage(Tok.metadata()), LHS(std::move(LHSExpression)),
      RHS(std::move(RHSExpression)),
      OperatorIDValue(static_cast<OperatorID>(Tok.id())) {
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

void BinaryExpressionASTNode::acceptASTPass(IASTPass &Pass) {}

void BinaryExpressionASTNode::print(std::ostream &Out) const {
  Out << "Binary Expression '" << MetadataStorage.get(common::Stringified)
      << "'";
  Out << " " << *LHS << " " << *RHS;
}

OperatorID BinaryExpressionASTNode::operatorID() const noexcept {
  return OperatorIDValue;
}

ExpressionSequenceASTNode::ExpressionSequenceASTNode(
    const common::Token &Tok, std::unique_ptr<IASTNode> FirstExpression,
    std::unique_ptr<IASTNode> SecondExpression)
    : MetadataStorage(Tok.metadata()), First(std::move(FirstExpression)),
      Second(std::move(SecondExpression)) {
  assert(Tok.id() == ExpressionSeparator &&
         "Expected expression separator token");
}

ExpressionSequenceASTNode::ExpressionSequenceASTNode(
    const ExpressionSequenceASTNode &RHS)
    : MetadataStorage(RHS.MetadataStorage), First(RHS.First->clone()),
      Second(RHS.Second->clone()) {}

ExpressionSequenceASTNode &
ExpressionSequenceASTNode::operator=(const ExpressionSequenceASTNode &RHS) {
  ExpressionSequenceASTNode NewASTNode(RHS);
  std::swap(*this, NewASTNode);
  return *this;
}

void ExpressionSequenceASTNode::acceptASTPass(IASTPass &Pass) {}

void ExpressionSequenceASTNode::print(std::ostream &Out) const {
  Out << "Expression Sequence '" << MetadataStorage.get(common::Stringified) << "'";
  Out << "\n" << *First << "\n" << *Second;
}

} // namespace cb
