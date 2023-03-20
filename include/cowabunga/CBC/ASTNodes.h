#ifndef COWABUNGA_CBC_ASTNODES_H
#define COWABUNGA_CBC_ASTNODES_H

#include "cowabunga/CBC/Definitions.h"
#include "cowabunga/Common/IClonableMixin.h"
#include "cowabunga/Common/IPrintable.h"

#include <memory>
#include <ostream>
#include <string>

namespace cb {

class Token;

class IASTPass;

class IASTNode : public IPrintable {
public:
  virtual void acceptASTPass(IASTPass &Pass) = 0;

  virtual std::unique_ptr<IASTNode> clone() const = 0;

  virtual ~IASTNode();
};

class VariableASTNode final : public IClonableMixin<IASTNode, VariableASTNode> {
public:
  VariableASTNode(const Token &Tok);

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::string Name;
};

class IntegralNumberASTNode final
    : public IClonableMixin<IASTNode, IntegralNumberASTNode> {
public:
  IntegralNumberASTNode(const Token &Tok);

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::string Value;
};

class BinaryExpressionASTNode final
    : public IClonableMixin<IASTNode, BinaryExpressionASTNode> {
public:
  BinaryExpressionASTNode(const Token &Tok,
                          std::unique_ptr<IASTNode> LHSExpression,
                          std::unique_ptr<IASTNode> RHSExpression);

  BinaryExpressionASTNode(const BinaryExpressionASTNode &RHSBinaryExpression);

  BinaryExpressionASTNode(
      BinaryExpressionASTNode &&RHSBinaryExpression) noexcept = default;

  BinaryExpressionASTNode &
  operator=(const BinaryExpressionASTNode &RHSBinaryExpression);

  BinaryExpressionASTNode &
  operator=(BinaryExpressionASTNode &&RHSBinaryExpression) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::unique_ptr<IASTNode> LHS;
  std::unique_ptr<IASTNode> RHS;
  std::string OperatorLexeme;
  OperatorID OpID;
};

class ParenthesizedExpressionASTNode final
    : public IClonableMixin<IASTNode, ParenthesizedExpressionASTNode> {
public:
  ParenthesizedExpressionASTNode(const Token &OpenParentheses,
                                 std::unique_ptr<IASTNode> InternalExpression,
                                 const Token &CloseParentheses);

  ParenthesizedExpressionASTNode(const ParenthesizedExpressionASTNode &RHS);

  ParenthesizedExpressionASTNode(
      ParenthesizedExpressionASTNode &&RHS) noexcept = default;

  ParenthesizedExpressionASTNode &
  operator=(const ParenthesizedExpressionASTNode &RHS);

  ParenthesizedExpressionASTNode &
  operator=(ParenthesizedExpressionASTNode &&RHS) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::unique_ptr<IASTNode> Expression;
  std::string OpenParenthesesLexeme;
  std::string CloseParenthesesLexeme;
};

class CompoundExpressionASTNode final
    : public IClonableMixin<IASTNode, CompoundExpressionASTNode> {
public:
  CompoundExpressionASTNode(const Token &ExpressionSeparator,
                            std::unique_ptr<IASTNode> FirstExpression,
                            std::unique_ptr<IASTNode> ReminingExpression);

  CompoundExpressionASTNode(const CompoundExpressionASTNode &RHS);

  CompoundExpressionASTNode(CompoundExpressionASTNode &&RHS) noexcept = default;

  CompoundExpressionASTNode &operator=(const CompoundExpressionASTNode &RHS);

  CompoundExpressionASTNode &
  operator=(CompoundExpressionASTNode &&RHS) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::unique_ptr<IASTNode> First;
  std::unique_ptr<IASTNode> Second;
  std::string ExpressionSeparatorLexeme;
};

} // namespace cb

#endif // COWABUNGA_CBC_ASTNODES_H
