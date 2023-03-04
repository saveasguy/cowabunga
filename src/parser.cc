#include "parser.h"

#include <sstream>
#include <utility>

#include "lexer.h"

namespace kaleidoc {

// --- EXCEPTIONS ---

UnknownAstBuilderRuleException::UnknownAstBuilderRuleException(std::string msg)
    : msg_{std::move(msg)} {}

const char *UnknownAstBuilderRuleException::what() const noexcept {
  return msg_.c_str();
}

// --- AST BUILDERS ---

// INTEGRAL NUMBER AST BUILDER

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
IntegralNumberAstBuilder::Build(std::vector<Token>::const_iterator begin,
                                std::vector<Token>::const_iterator end) const {
  if (begin == end || begin->id() != TokenId::kIntegralNumber) {
    return std::make_pair(nullptr, ++begin);
  }
  long long value = std::stoll(begin->stringified());
  return std::make_pair(std::make_unique<IntegralNumberAstNode>(value),
                        begin + 1);
}

std::unique_ptr<AstBuilder> IntegralNumberAstBuilder::Clone() const {
  return std::make_unique<IntegralNumberAstBuilder>(*this);
}

// VARIABLE AST BUILDER

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
VariableAstBuilder::Build(std::vector<Token>::const_iterator begin,
                          std::vector<Token>::const_iterator end) const {
  if (begin == end || begin->id() != TokenId::kIdentifier) {
    return std::make_pair(nullptr, begin);
  }
  return std::make_pair(std::make_unique<VariableAstNode>(begin->stringified()),
                        begin + 1);
}

std::unique_ptr<AstBuilder> VariableAstBuilder::Clone() const {
  return std::make_unique<VariableAstBuilder>(*this);
}

// PRIMARY EXPRESSION AST BUILDER

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
PrimaryExpressionAstBuilder::Build(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end) const {
  for (const auto &builder : ast_builders_) {
    auto result = builder->Build(begin, end);
    if (result.first) { return result; }
  }
  return std::make_pair(nullptr, begin);
}

void PrimaryExpressionAstBuilder::AddBuilder(AstBuilder *builder) {
  ast_builders_.push_back(builder);
}

std::unique_ptr<AstBuilder> PrimaryExpressionAstBuilder::Clone() const {
  return std::make_unique<PrimaryExpressionAstBuilder>(*this);
}

// BINARY EXPRESSION AST BUILDER

BinaryExpressionAstBuilder::BinaryExpressionAstBuilder()
    : primary_builder_{nullptr} {
  operator_precedence_[OperatorId::kMinusOp] = kMaxPrecedence / 2;
  operator_precedence_[OperatorId::kPlusOp] = kMaxPrecedence / 2;
  operator_precedence_[OperatorId::kShiftLeftOp] = kMaxPrecedence / 4;
  operator_precedence_[OperatorId::kShiftRightOp] = kMaxPrecedence / 4;
  operator_precedence_[OperatorId::kAssignmentOp] = 1;
}

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
BinaryExpressionAstBuilder::Build(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end) const {
  auto lhs_result = primary_builder_->Build(begin, end);
  if (!lhs_result.first) { return lhs_result; }
  auto binop_result =
      BuildBinopRhs(lhs_result.second, end, std::move(lhs_result.first));
  if (!binop_result.first) { return std::make_pair(nullptr, begin); }
  return binop_result;
}

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
BinaryExpressionAstBuilder::BuildBinopRhs(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end,
    std::unique_ptr<AstNode> lhs) const {
  if (begin == end || !TokenIsBinaryOperator(*begin)) {
    return std::make_pair(nullptr, begin);
  }
  return BuildBinopRhsChain(begin, end, std::move(lhs));
}

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
BinaryExpressionAstBuilder::BuildBinopRhsChain(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end,
    std::unique_ptr<AstNode> lhs) const {
  auto operator_str = begin->stringified();
  auto operator_id = static_cast<OperatorId>(begin->id());
  int binop_precedence = operator_precedence_.at(operator_id);
  auto rhs_result = BuildRhsPrimary(begin + 1, end);
  begin = rhs_result.second;
  if (begin == end || !TokenIsBinaryOperator(*begin)) {
    return std::make_pair(std::make_unique<BinaryExpressionAstNode>(
                              operator_id, std::move(lhs),
                              std::move(rhs_result.first), operator_str),
                          begin);
  }
  auto next_operator_id = static_cast<OperatorId>(begin->id());
  int next_binop_precedence = operator_precedence_.at(next_operator_id);
  if (binop_precedence > next_binop_precedence) {
    auto lhs_ast = std::make_unique<BinaryExpressionAstNode>(
        operator_id, std::move(lhs), std::move(rhs_result.first), operator_str);
    return BuildBinopRhsChain(begin, end, std::move(lhs_ast));
  }
  auto binop_rhs_result =
      BuildBinopRhsChain(begin, end, std::move(rhs_result.first));
  return std::make_pair(std::make_unique<BinaryExpressionAstNode>(
                            operator_id, std::move(lhs),
                            std::move(binop_rhs_result.first), operator_str),
                        binop_rhs_result.second);
}

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
BinaryExpressionAstBuilder::BuildRhsPrimary(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end) const {
  auto result = primary_builder_->Build(begin, end);
  if (!result.first) {
    std::stringstream str;
    if (result.second == end) {
      str << "Unexpected end of line!";
    } else {
      str << "Unexpected token: " << begin->stringified();
    }
    throw UnknownAstBuilderRuleException{str.str()};
  }
  return result;
}

bool BinaryExpressionAstBuilder::TokenIsBinaryOperator(
    const Token &token) noexcept {
  return token.id() > TokenId::kBinaryOperatorsRangeBegin &&
         token.id() < TokenId::kBinaryOperatorsRangeEnd;
}

void BinaryExpressionAstBuilder::SetPrimaryBuilder(AstBuilder *builder) {
  primary_builder_ = builder;
}

std::unique_ptr<AstBuilder> BinaryExpressionAstBuilder::Clone() const {
  return std::make_unique<BinaryExpressionAstBuilder>(*this);
}

// --- AST NODES ---

// EXPRESSION SEQUENCE AST NODE

ExpressionSequenceAstNode::ExpressionSequenceAstNode(
    std::unique_ptr<AstNode> lhs, std::unique_ptr<AstNode> rhs)
    : lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {}

ExpressionSequenceAstNode::ExpressionSequenceAstNode(
    const ExpressionSequenceAstNode &rhs)
    : lhs_{rhs.lhs_->Clone()}, rhs_{rhs.rhs_->Clone()} {}

ExpressionSequenceAstNode &ExpressionSequenceAstNode::operator=(
    const ExpressionSequenceAstNode &rhs) {
  ExpressionSequenceAstNode new_node{rhs};
  std::swap(*this, new_node);
  return *this;
}

std::unique_ptr<AstNode> ExpressionSequenceAstNode::Clone() const {
  return std::make_unique<ExpressionSequenceAstNode>(*this);
}

// INTEGRAL NUMBER AST NODE

IntegralNumberAstNode::IntegralNumberAstNode(long long value) : value_{value} {}

void IntegralNumberAstNode::Print(std::ostream &out) const {
  out << "Integer '" << value_ << "'";
}

std::unique_ptr<AstNode> IntegralNumberAstNode::Clone() const {
  return std::make_unique<IntegralNumberAstNode>(*this);
}

// VARIABLE AST NODE

VariableAstNode::VariableAstNode(std::string name) : name_{std::move(name)} {}

void VariableAstNode::Print(std::ostream &out) const {
  out << "Variable '" << name_ << "'";
}

std::unique_ptr<AstNode> VariableAstNode::Clone() const {
  return std::make_unique<VariableAstNode>(*this);
};

// BINARY EXPRESSION AST NODE

BinaryExpressionAstNode::BinaryExpressionAstNode(OperatorId op,
                                                 std::unique_ptr<AstNode> lhs,
                                                 std::unique_ptr<AstNode> rhs,
                                                 std::string operator_str)
    : op_{op},
      lhs_{std::move(lhs)},
      rhs_{std::move(rhs)},
      stringifed_operator_{std::move(operator_str)} {}

BinaryExpressionAstNode::BinaryExpressionAstNode(
    const BinaryExpressionAstNode &rhs)
    : op_{rhs.op_}, lhs_{rhs.lhs_->Clone()}, rhs_{rhs.rhs_->Clone()} {}

BinaryExpressionAstNode &BinaryExpressionAstNode::operator=(
    const BinaryExpressionAstNode &rhs) {
  BinaryExpressionAstNode new_node{rhs};
  std::swap(*this, new_node);
  return *this;
}

OperatorId BinaryExpressionAstNode::operator_id() const { return op_; }

void BinaryExpressionAstNode::Print(std::ostream &out) const {
  out << stringifed_operator_ << "\n/" << *lhs_ << "\n\\" << *rhs_;
}

std::unique_ptr<AstNode> BinaryExpressionAstNode::Clone() const {
  return std::make_unique<BinaryExpressionAstNode>(*this);
}

}  // namespace kaleidoc