#ifndef COWABUNGA_CBC_PARSERS_H
#define COWABUNGA_CBC_PARSERS_H

#include "cowabunga/CBC/ASTBuilder.h"
#include "cowabunga/Common/IClonableMixin.h"
#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/ICFGRule.h"

namespace cb {

enum NonTerminalID {
  NTID_TopLevelExpression,
  NTID_CompoundExpression,
  NTID_Expression,
  NTID_RValue,
  NTID_LValue,
  NTID_ParamList
};

class ParamListToParamList final
    : public IClonableMixin<ICFGRule, ParamListToParamList> {
public:
  ParamListToParamList(const Lexer &LexImpl,
                                         ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class ParamListToParam final
    : public IClonableMixin<ICFGRule, ParamListToParam> {
public:
  ParamListToParam(const Lexer &LexImpl,
                                         ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class TopLevelExpressionToCompoundExpression final
    : public IClonableMixin<ICFGRule, TopLevelExpressionToCompoundExpression> {
public:
  TopLevelExpressionToCompoundExpression(const Lexer &LexImpl,
                                         ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class CompoundExpressionToExpressionSequence final
    : public IClonableMixin<ICFGRule, CompoundExpressionToExpressionSequence> {
public:
  CompoundExpressionToExpressionSequence(const Lexer &LexImpl,
                                         ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class CompoundExpressionToSingleExpression final
    : public IClonableMixin<ICFGRule, CompoundExpressionToSingleExpression> {
public:
  CompoundExpressionToSingleExpression(const Lexer &LexImpl,
                                       ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class ExpressionToAssignment final
    : public IClonableMixin<ICFGRule, ExpressionToAssignment> {
public:
  ExpressionToAssignment(const Lexer &LexImpl, ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class ExpressionToRValue final
    : public IClonableMixin<ICFGRule, ExpressionToRValue> {
public:
  ExpressionToRValue(const Lexer &LexImpl, ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class RValueToCall final : public IClonableMixin<ICFGRule, RValueToCall> {
public:
  RValueToCall(const Lexer &LexImpl, ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class RValueToLValue final : public IClonableMixin<ICFGRule, RValueToLValue> {
public:
  RValueToLValue(const Lexer &LexImpl, ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class RValueToIntegralNumber final
    : public IClonableMixin<ICFGRule, RValueToIntegralNumber> {
public:
  RValueToIntegralNumber(const Lexer &LexImpl, ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

class LValueToIdentifier final
    : public IClonableMixin<ICFGRule, LValueToIdentifier> {
public:
  LValueToIdentifier(const Lexer &LexImpl, ASTBuilder &ASTBulderObject);

  void parse(TokenIterator ItToken) override;

  void produceError(CFGParserError Error) override;

  Symbol getLHSNonTerminal() const override;

  std::vector<Symbol> getProducts() const override;

private:
  const Lexer *Lex;
  ASTBuilder *Builder;
};

} // namespace cb

#endif // COWABUNGA_CBC_PARSERS_H
