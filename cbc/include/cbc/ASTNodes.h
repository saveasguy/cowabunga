#ifndef CBC_ASTNODES_H
#define CBC_ASTNODES_H

#include "cbc/Driver.h"
#include "common/IClonableMixin.h"
#include "common/IPrintable.h"
#include "common/Metadata.h"
#include "common/Token.h"

#include <bits/types/cookie_io_functions_t.h>
#include <memory>
#include <ostream>
#include <string>

namespace cb {

class IASTPass;

class IASTNode : public common::IPrintable {
public:
  virtual void acceptASTPass(IASTPass &Pass) = 0;

  virtual std::unique_ptr<IASTNode> clone() const = 0;

  virtual ~IASTNode() = default;
};

class VariableASTNode final
    : public common::IClonableMixin<IASTNode, VariableASTNode> {
public:
  VariableASTNode(const common::Token &Tok);

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  common::Metadata MetadataStorage;
  std::string Name;
};

class IntegralNumberASTNode final
    : public common::IClonableMixin<IASTNode, IntegralNumberASTNode> {
public:
  IntegralNumberASTNode(const common::Token &Tok);

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  common::Metadata MetadataStorage;
  long long Value;
};

class BinaryExpressionASTNode final
    : public common::IClonableMixin<IASTNode, BinaryExpressionASTNode> {
public:
  BinaryExpressionASTNode(const common::Token &Tok,
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

  common::Metadata MetadataStorage;
  std::unique_ptr<IASTNode> LHS;
  std::unique_ptr<IASTNode> RHS;
  OperatorID OpID;
};

class ParenthesizedExpressionASTNode final
    : public common::IClonableMixin<IASTNode, ParenthesizedExpressionASTNode> {
public:
  ParenthesizedExpressionASTNode(const common::Token &OpenParentheses,
                                 std::unique_ptr<IASTNode> InternalExpression,
                                 const common::Token &CloseParentheses);

  ParenthesizedExpressionASTNode(const ParenthesizedExpressionASTNode &RHS);

  ParenthesizedExpressionASTNode(
      ParenthesizedExpressionASTNode &&RHS) noexcept = default;

  ParenthesizedExpressionASTNode &
  operator=(const ParenthesizedExpressionASTNode &RHS);

  ParenthesizedExpressionASTNode &
  operator=(ParenthesizedExpressionASTNode &&RHS) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  common::Metadata MetadataStorage;
  std::unique_ptr<IASTNode> Expression;
};

class CompoundExpressionASTNode final
    : public common::IClonableMixin<IASTNode, CompoundExpressionASTNode> {
public:
  CompoundExpressionASTNode(const common::Token &ExpressionSeparator,
                            std::unique_ptr<IASTNode> FirstExpression,
                            std::unique_ptr<IASTNode> ReminingExpression);

  CompoundExpressionASTNode(const CompoundExpressionASTNode &RHS);

  CompoundExpressionASTNode(CompoundExpressionASTNode &&RHS) noexcept = default;

  CompoundExpressionASTNode &operator=(const CompoundExpressionASTNode &RHS);

  CompoundExpressionASTNode &
  operator=(CompoundExpressionASTNode &&RHS) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  common::Metadata MetadataStorage;
  std::unique_ptr<IASTNode> First;
  std::unique_ptr<IASTNode> Second;
};

} // namespace cb

#endif
