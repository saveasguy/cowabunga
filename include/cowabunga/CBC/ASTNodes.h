#ifndef COWABUNGA_CBC_ASTNODES_H
#define COWABUNGA_CBC_ASTNODES_H

#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Common/IClonableMixin.h"
#include "cowabunga/Common/IPrintable.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

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

class AssignmentExpressionASTNode final : public IClonableMixin<IASTNode, AssignmentExpressionASTNode> {
public:
  AssignmentExpressionASTNode(std::string Assignment,
                            std::unique_ptr<IASTNode> LHSNode, std::unique_ptr<IASTNode> RHSNode);

  AssignmentExpressionASTNode(const AssignmentExpressionASTNode &RHS);

  AssignmentExpressionASTNode(AssignmentExpressionASTNode &&RHS) noexcept = default;

  AssignmentExpressionASTNode &operator=(const AssignmentExpressionASTNode &RHS);

  AssignmentExpressionASTNode &
  operator=(AssignmentExpressionASTNode &&RHS) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::unique_ptr<IASTNode> LHS, RHS;
  std::string AssignmentLexeme;
};

class CompoundExpressionASTNode final
    : public IClonableMixin<IASTNode, CompoundExpressionASTNode> {
public:
  CompoundExpressionASTNode(std::string ExpressionSeparator,
                            std::vector<std::unique_ptr<IASTNode>> ExpressionList);

  CompoundExpressionASTNode(const CompoundExpressionASTNode &RHS);

  CompoundExpressionASTNode(CompoundExpressionASTNode &&RHS) noexcept = default;

  CompoundExpressionASTNode &operator=(const CompoundExpressionASTNode &RHS);

  CompoundExpressionASTNode &
  operator=(CompoundExpressionASTNode &&RHS) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::vector<std::unique_ptr<IASTNode>> Expressions;
  std::string ExpressionSeparatorLexeme;
};

class CallExpressionASTNode final
    : public IClonableMixin<IASTNode, CallExpressionASTNode> {
public:
  CallExpressionASTNode(const Token &Tok,
                            std::vector<std::unique_ptr<IASTNode>> ParamList);

  CallExpressionASTNode(const CallExpressionASTNode &RHS);

  CallExpressionASTNode(CallExpressionASTNode &&RHS) noexcept = default;

  CallExpressionASTNode &operator=(const CallExpressionASTNode &RHS);

  CallExpressionASTNode &
  operator=(CallExpressionASTNode &&RHS) noexcept = default;

  void acceptASTPass(IASTPass &Pass) override;

  void print(std::ostream &Out) const override;

  std::vector<std::unique_ptr<IASTNode>> Parameters;
  std::string FuncName;
};

} // namespace cb

#endif // COWABUNGA_CBC_ASTNODES_H
