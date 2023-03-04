#ifndef KALEIDOC_SRC_PARSER_H_
#define KALEIDOC_SRC_PARSER_H_

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "driver.h"

namespace kaleidoc {

// --- EXCEPTIONS ---

class UnknownAstBuilderRuleException : public std::exception {
 public:
  UnknownAstBuilderRuleException(std::string msg);

  const char *what() const noexcept override;

  ~UnknownAstBuilderRuleException() override = default;

 private:
  std::string msg_;
};

// --- AST BUILDERS ---

class IntegralNumberAstBuilder : public AstBuilder {
 public:
  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator> Build(
      std::vector<Token>::const_iterator begin,
      std::vector<Token>::const_iterator end) const override;

  std::unique_ptr<AstBuilder> Clone() const override;
};

class VariableAstBuilder : public AstBuilder {
 public:
  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator> Build(
      std::vector<Token>::const_iterator begin,
      std::vector<Token>::const_iterator end) const override;

  std::unique_ptr<AstBuilder> Clone() const override;
};

class PrimaryExpressionAstBuilder : public AstBuilder {
 public:
  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator> Build(
      std::vector<Token>::const_iterator begin,
      std::vector<Token>::const_iterator end) const override;

  void AddBuilder(AstBuilder *builder);

  std::unique_ptr<AstBuilder> Clone() const override;

 private:
  std::vector<AstBuilder *> ast_builders_;
};

class BinaryExpressionAstBuilder : public AstBuilder {
 public:
  BinaryExpressionAstBuilder();

  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator> Build(
      std::vector<Token>::const_iterator begin,
      std::vector<Token>::const_iterator end) const override;

  void SetPrimaryBuilder(AstBuilder *builder);

  std::unique_ptr<AstBuilder> Clone() const override;

 private:
  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
  BuildBinopRhs(std::vector<Token>::const_iterator begin,
                std::vector<Token>::const_iterator end,
                std::unique_ptr<AstNode> lhs) const;

  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
  BuildBinopRhsChain(std::vector<Token>::const_iterator begin,
                     std::vector<Token>::const_iterator end,
                     std::unique_ptr<AstNode> lhs) const;

  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
  BuildRhsPrimary(std::vector<Token>::const_iterator begin,
                  std::vector<Token>::const_iterator end) const;

  static bool TokenIsBinaryOperator(const Token &token) noexcept;

  static constexpr int kMaxPrecedence = 8;
  std::map<OperatorId, int> operator_precedence_;
  AstBuilder *primary_builder_;
};

// --- AST NODES ---

class ExpressionSequenceAstNode : public AstNode {
 public:
  ExpressionSequenceAstNode(std::unique_ptr<AstNode> lhs,
                            std::unique_ptr<AstNode> rhs);

  ExpressionSequenceAstNode(const ExpressionSequenceAstNode &rhs);

  ExpressionSequenceAstNode(ExpressionSequenceAstNode &&rhs) noexcept = default;

  ExpressionSequenceAstNode &operator=(const ExpressionSequenceAstNode &rhs);

  ExpressionSequenceAstNode &operator=(
      ExpressionSequenceAstNode &&rhs) noexcept = default;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::unique_ptr<AstNode> lhs_;
  std::unique_ptr<AstNode> rhs_;
};

class IntegralNumberAstNode : public AstNode {
 public:
  IntegralNumberAstNode(long long value);

  void Print(std::ostream &out) const override;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  long long value_;
};

class VariableAstNode : public AstNode {
 public:
  VariableAstNode(std::string name);

  void Print(std::ostream &out) const override;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::string name_;
};

class BinaryExpressionAstNode : public AstNode {
 public:
  BinaryExpressionAstNode(OperatorId op, std::unique_ptr<AstNode> lhs,
                          std::unique_ptr<AstNode> rhs,
                          std::string operator_str = "Unknown");

  BinaryExpressionAstNode(const BinaryExpressionAstNode &rhs);

  BinaryExpressionAstNode(BinaryExpressionAstNode &&rhs) noexcept = default;

  BinaryExpressionAstNode &operator=(const BinaryExpressionAstNode &rhs);

  BinaryExpressionAstNode &operator=(BinaryExpressionAstNode &&rhs) noexcept =
      default;

  OperatorId operator_id() const;

  void Print(std::ostream &out) const override;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  OperatorId op_;
  std::string stringifed_operator_;
  std::unique_ptr<AstNode> lhs_;
  std::unique_ptr<AstNode> rhs_;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_PARSER_H_
