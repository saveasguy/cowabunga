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

class UnexpectedTokenException : public std::exception {
 public:
  UnexpectedTokenException(std::string msg);

  const char *what() const noexcept override;

  ~UnexpectedTokenException() override = default;

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
                std::unique_ptr<AstNode> lhs,
                int expression_precedence = -1) const;

  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator>
  BuildRhsPrimary(std::vector<Token>::const_iterator begin,
                  std::vector<Token>::const_iterator end) const;

  bool PrecedenceLessOrEqualAndLhsRightAssociative(const Token &lhs,
                                                   const Token &rhs) const;

  bool PrecedenceGreaterOrEqualAndRightAssociative(const Token &lhs,
                                                   int rhs_precedence) const;

  int CompareBinopTokenPrecedences(const Token &lhs, const Token &rhs) const;

  int CompareBinopTokenPrecedences(const Token &lhs, int rhs_precedence) const;

  int BinaryOperatorTokenPrecedence(const Token &token) const;

  static bool TokenIsBinaryOperator(const Token &token) noexcept;

  std::map<OperatorId, int> operator_precedence_;
  std::map<OperatorId, OperatorAssociativity> operator_associativity_;
  AstBuilder *primary_builder_;
};

class ExpressionSequenceAstBuilder : public AstBuilder {
 public:
  std::pair<std::unique_ptr<AstNode>, std::vector<Token>::const_iterator> Build(
      std::vector<Token>::const_iterator begin,
      std::vector<Token>::const_iterator end) const override;

  void AddExpressionBuilder(AstBuilder *builder);

  std::unique_ptr<AstBuilder> Clone() const override;

 private:
  std::vector<AstBuilder *> expression_builders_;
};

// --- AST NODES ---

class IntegralNumberAstNode : public AstNode {
 public:
  IntegralNumberAstNode(const Token &token);

  void Print(std::ostream &out) const override;

  std::unique_ptr<AstNode> Clone() const override;
};

class VariableAstNode : public AstNode {
 public:
  VariableAstNode(const Token &token);

  void Print(std::ostream &out) const override;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::string name_;
};

class BinaryExpressionAstNode : public AstNode {
 public:
  BinaryExpressionAstNode(const Token &token, std::unique_ptr<AstNode> lhs,
                          std::unique_ptr<AstNode> rhs);

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
  std::unique_ptr<AstNode> lhs_;
  std::unique_ptr<AstNode> rhs_;
};

class ExpressionSequenceAstNode : public AstNode {
 public:
  ExpressionSequenceAstNode(const Token &expression_separator_token,
                            std::unique_ptr<AstNode> lhs_expression,
                            std::unique_ptr<AstNode> rhs_expression);

  ExpressionSequenceAstNode(const ExpressionSequenceAstNode &rhs);

  ExpressionSequenceAstNode(ExpressionSequenceAstNode &&rhs) noexcept = default;

  ExpressionSequenceAstNode &operator=(const ExpressionSequenceAstNode &rhs);

  ExpressionSequenceAstNode &operator=(
      ExpressionSequenceAstNode &&rhs) noexcept = default;

  void Print(std::ostream &out) const override;

  std::unique_ptr<AstNode> Clone() const override;

 private:
  std::unique_ptr<AstNode> lhs_expression_;
  std::unique_ptr<AstNode> rhs_expression_;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_PARSER_H_
