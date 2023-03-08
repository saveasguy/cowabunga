#include "parser.h"

#include <cstdlib>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "driver.h"
#include "lexer.h"

namespace kaleidoc {

// --- EXCEPTIONS ---

UnknownAstBuilderRuleException::UnknownAstBuilderRuleException(std::string msg)
    : msg_{std::move(msg)} {}

const char *UnknownAstBuilderRuleException::what() const noexcept {
  return msg_.c_str();
}

UnexpectedTokenException::UnexpectedTokenException(std::string msg)
    : msg_{std::move(msg)} {}

const char *UnexpectedTokenException::what() const noexcept {
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

  return std::make_pair(std::make_unique<IntegralNumberAstNode>(*begin),
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
  return std::make_pair(std::make_unique<VariableAstNode>(*begin), begin + 1);
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
    : primary_builder_{nullptr},
      operator_precedence_{GetOperatorPrecedenceValues()},
      operator_associativity_{GetOperatorAssociativityValues()} {}

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
BinaryExpressionAstBuilder::Build(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end) const {
  auto result = primary_builder_->Build(begin, end);
  if (!result.first || result.second == end ||
      !TokenIsBinaryOperator(*result.second)) {
    return std::make_pair(nullptr, begin);
  }
  auto binop_rhs_iterator = result.second;
  result = BuildBinopRhs(binop_rhs_iterator, end, std::move(result.first));
  if (result.second == binop_rhs_iterator) {
    return std::make_pair(nullptr, begin);
  }
  return result;
}

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
BinaryExpressionAstBuilder::BuildBinopRhs(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end, std::unique_ptr<AstNode> lhs,
    int expression_precedence) const {
  while (begin != end && TokenIsBinaryOperator(*begin) &&
         PrecedenceGreaterOrEqualAndRightAssociative(*begin,
                                                     expression_precedence)) {
    auto op_token = begin;
    ++begin;
    auto rhs_result = BuildRhsPrimary(begin, end);
    begin = rhs_result.second;
    if (TokenIsBinaryOperator(*begin) &&
        PrecedenceLessOrEqualAndLhsRightAssociative(*op_token, *begin)) {
      int op_token_precedence = BinaryOperatorTokenPrecedence(*op_token);
      rhs_result = BuildBinopRhs(begin, end, std::move(rhs_result.first),
                                 op_token_precedence);
      begin = rhs_result.second;
    }
    lhs = std::make_unique<BinaryExpressionAstNode>(
        *op_token, std::move(lhs), std::move(rhs_result.first));
    auto op_token_precedence = BinaryOperatorTokenPrecedence(*op_token);
  }
  return std::make_pair(std::move(lhs), begin);
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
    } else if (begin->metadata().count(MetadataType::kStringified)) {
      str << "Unexpected token: "
          << begin->metadata().at(MetadataType::kStringified);
    }
    throw UnknownAstBuilderRuleException{str.str()};
  }
  return result;
}

bool BinaryExpressionAstBuilder::PrecedenceLessOrEqualAndLhsRightAssociative(
    const Token &lhs, const Token &rhs) const {
  int comparison_result = CompareBinopTokenPrecedences(lhs, rhs);
  auto lhs_operator_id = static_cast<OperatorId>(lhs.id());
  if (operator_associativity_.at(lhs_operator_id) ==
      OperatorAssociativity::kLeftToRight) {
    return comparison_result < 0;
  }
  return comparison_result <= 0;
}

bool BinaryExpressionAstBuilder::PrecedenceGreaterOrEqualAndRightAssociative(
    const Token &lhs, int rhs_precedence) const {
  int comparison_result = CompareBinopTokenPrecedences(lhs, rhs_precedence);
  auto lhs_operator_id = static_cast<OperatorId>(lhs.id());
  if (operator_associativity_.at(lhs_operator_id) ==
      OperatorAssociativity::kLeftToRight) {
    return comparison_result > 0;
  }
  return comparison_result >= 0;
}

int BinaryExpressionAstBuilder::CompareBinopTokenPrecedences(
    const Token &lhs, const Token &rhs) const {
  return BinaryOperatorTokenPrecedence(lhs) -
         BinaryOperatorTokenPrecedence(rhs);
}

int BinaryExpressionAstBuilder::CompareBinopTokenPrecedences(
    const Token &lhs, int rhs_precedence) const {
  return BinaryOperatorTokenPrecedence(lhs) - rhs_precedence;
}

int BinaryExpressionAstBuilder::BinaryOperatorTokenPrecedence(
    const Token &token) const {
  auto operator_id = static_cast<OperatorId>(token.id());
  return operator_precedence_.at(operator_id);
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

// EXPRESSION SEQUENCE AST BUILDER

std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
ExpressionSequenceAstBuilder::Build(
    std::vector<Token>::const_iterator begin,
    std::vector<Token>::const_iterator end) const {
  if (begin == end) { return std::make_pair(nullptr, begin); }
  std::unique_ptr<AstNode> lhs_ast;
  auto it = begin;
  for (auto *builder : expression_builders_) {
    auto temp_result = builder->Build(begin, end);
    if (temp_result.first && temp_result.second - it > 0) {
      lhs_ast = std::move(temp_result.first);
      it = temp_result.second;
    }
  }
  if (it == end || it->id() != TokenId::kExpressionSeparator) {
    return std::make_pair(nullptr, begin);
  }
  auto sequence_separator_it = it;
  auto rhs_result = Build(it + 1, end);
  auto sequence_expr = std::make_unique<ExpressionSequenceAstNode>(
      *sequence_separator_it, std::move(lhs_ast), std::move(rhs_result.first));
  return std::make_pair(std::move(sequence_expr), rhs_result.second);
}

void ExpressionSequenceAstBuilder::AddExpressionBuilder(AstBuilder *builder) {
  expression_builders_.push_back(builder);
}

std::unique_ptr<AstBuilder> ExpressionSequenceAstBuilder::Clone() const {
  return std::make_unique<ExpressionSequenceAstBuilder>(*this);
}

// --- AST NODES ---

// INTEGRAL NUMBER AST NODE

IntegralNumberAstNode::IntegralNumberAstNode(const Token &token)
    : AstNode{token.metadata()} {
  if (token.id() != TokenId::kIntegralNumber) {
    std::stringstream str;
    str << "Expected Token {" << TokenId::kIntegralNumber << "}, got " << token;
    throw UnexpectedTokenException{str.str()};
  }
}

void IntegralNumberAstNode::Print(std::ostream &out) const {
  auto it = metadata_.find(MetadataType::kStringified);
  if (it != metadata_.cend()) {
    out << "Integer '" << it->second << "'";
  } else {
    out << "Integer '*unknown*'";
  }
}

void IntegralNumberAstNode::AcceptAstPrinter(AstPrinter *printer) const {}

std::unique_ptr<AstNode> IntegralNumberAstNode::Clone() const {
  return std::make_unique<IntegralNumberAstNode>(*this);
}

// VARIABLE AST NODE

VariableAstNode::VariableAstNode(const Token &token)
    : AstNode{token.metadata()} {
  if (token.id() != TokenId::kIdentifier) {
    std::stringstream str;
    str << "Expected Token {" << TokenId::kIdentifier << "}, got " << token;
    throw UnexpectedTokenException{str.str()};
  }
}

void VariableAstNode::Print(std::ostream &out) const {
  auto it = metadata_.find(MetadataType::kStringified);
  if (it != metadata_.cend()) {
    out << "Variable '" << it->second << "'";
  } else {
    out << "Variable '*unknown*'";
  }
}

void VariableAstNode::AcceptAstPrinter(AstPrinter *printer) const {}

std::unique_ptr<AstNode> VariableAstNode::Clone() const {
  return std::make_unique<VariableAstNode>(*this);
};

// BINARY EXPRESSION AST NODE

BinaryExpressionAstNode::BinaryExpressionAstNode(const Token &token,
                                                 std::unique_ptr<AstNode> lhs,
                                                 std::unique_ptr<AstNode> rhs)
    : AstNode{token.metadata()}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {
  if (token.id() < TokenId::kBinaryOperatorsRangeBegin ||
      token.id() > TokenId::kBinaryOperatorsRangeEnd) {
    std::stringstream str;
    str << "Expected Token { binary_expression }, got " << token;
    throw UnexpectedTokenException{str.str()};
  }
  op_ = static_cast<OperatorId>(token.id());
}

BinaryExpressionAstNode::BinaryExpressionAstNode(
    const BinaryExpressionAstNode &rhs)
    : AstNode{rhs.metadata_},
      op_{rhs.op_},
      lhs_{rhs.lhs_->Clone()},
      rhs_{rhs.rhs_->Clone()} {}

BinaryExpressionAstNode &BinaryExpressionAstNode::operator=(
    const BinaryExpressionAstNode &rhs) {
  BinaryExpressionAstNode new_node{rhs};
  std::swap(*this, new_node);
  return *this;
}

OperatorId BinaryExpressionAstNode::operator_id() const { return op_; }

void BinaryExpressionAstNode::Print(std::ostream &out) const {
  auto it = metadata_.find(MetadataType::kStringified);
  if (it != metadata_.cend()) {
    out << "BinaryOperator '" << it->second << "'";
  } else {
    out << "BinaryOperator '*unknown*'";
  }
}

void BinaryExpressionAstNode::AcceptAstPrinter(AstPrinter *printer) const {
  printer->PrintAst(lhs_.get());
  printer->PrintAst(rhs_.get());
}

std::unique_ptr<AstNode> BinaryExpressionAstNode::Clone() const {
  return std::make_unique<BinaryExpressionAstNode>(*this);
}

// EXPRESSION SEQUENCE AST NODE

ExpressionSequenceAstNode::ExpressionSequenceAstNode(
    const Token &expression_separator_token,
    std::unique_ptr<AstNode> lhs_expression,
    std::unique_ptr<AstNode> rhs_expression)
    : AstNode{expression_separator_token.metadata()},
      lhs_expression_{std::move(lhs_expression)},
      rhs_expression_{std::move(rhs_expression)} {}

ExpressionSequenceAstNode::ExpressionSequenceAstNode(
    const ExpressionSequenceAstNode &rhs)
    : AstNode{rhs.metadata_},
      lhs_expression_{rhs.lhs_expression_->Clone()},
      rhs_expression_{rhs.rhs_expression_->Clone()} {}

ExpressionSequenceAstNode &ExpressionSequenceAstNode::operator=(
    const ExpressionSequenceAstNode &rhs) {
  ExpressionSequenceAstNode new_node{rhs};
  std::swap(new_node, *this);
  return *this;
}

void ExpressionSequenceAstNode::Print(std::ostream &out) const {
  auto it = metadata_.find(MetadataType::kStringified);
  if (it != metadata_.cend()) {
    out << "ExpressionSequence '" << it->second << "'";
  } else {
    out << "ExpressionSequence '*unknown*'";
  }
}

void ExpressionSequenceAstNode::AcceptAstPrinter(AstPrinter *printer) const {
  printer->PrintAst(lhs_expression_.get());
  if (rhs_expression_) { printer->PrintAst(rhs_expression_.get()); }
}

std::unique_ptr<AstNode> ExpressionSequenceAstNode::Clone() const {
  return std::make_unique<ExpressionSequenceAstNode>(*this);
}

// --- AST PRINTERS ---

// Ladder Ast Printer

LadderAstPrinter::LadderAstPrinter(std::ostream &out) : out_{&out} {}

void LadderAstPrinter::PrintAst(AstNode *node) {
  for (int i = 0; i < depth_; ++i) { *out_ << "--"; }
  node->Print(*out_);
  *out_ << std::endl;
  ++depth_;
  node->AcceptAstPrinter(this);
  --depth_;
}

}  // namespace kaleidoc