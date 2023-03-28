#include "cowabunga/CBC/Parsers.h"
#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/CFGParserError.h"
#include "cowabunga/Parser/Symbol.h"

#include <iostream>

using namespace cb;

namespace {

void printError(CFGParserError Error, const Lexer &Lex) {
  auto LastToken = Error.ItFoundToken;
  auto ColumnNumber = LastToken->BeginColumnNumber;
  if (Error.EOFFound) {
    ColumnNumber = LastToken->EndColumnNumber;
  }
  std::cerr << LastToken->getFile() << ":" << LastToken->LineNumber << ":"
            << ColumnNumber << ": unexpected token\n";
  if (Error.EOFFound) {
    std::cerr << "\t";
    if (Error.ExpectedSymbol.isTerminal()) {
      std::cerr << "expected "
                << Lex.getTokenLexeme(
                       static_cast<TokenID>(Error.ExpectedSymbol.getID()))
                << ", ";
    }
    std::cerr << "found EOF\n";
    exit(2);
  }
  if (Error.EOFExpected) {
    std::cerr << "\t" << Error.ItFoundToken->getLine() << "\n\t";
    for (size_t I = 0; I < Error.ItFoundToken->BeginColumnNumber - 1; ++I) {
      std::cerr << " ";
    }
    std::cerr << "^\n";
    std::cerr << "expected EOF, found " << Error.ItFoundToken->getLexeme()
              << "\n";
    exit(2);
  }
  std::cerr << "\t" << Error.ItFoundToken->getLine() << "\n\t";
  for (size_t I = 0; I < Error.ItFoundToken->BeginColumnNumber - 1; ++I) {
    std::cerr << " ";
  }
  std::cerr << "^\n";
  std::cerr << "expected "
            << Lex.getTokenLexeme(
                   static_cast<TokenID>(Error.ExpectedSymbol.getID()))
            << ", found " << Error.ItFoundToken->getLexeme() << "\n";
  exit(2);
}

} // namespace

ParamListToParamList::ParamListToParamList(const Lexer &LexImpl,
                                           ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void ParamListToParamList::parse(TokenIterator ItToken) {
  Builder->createParameter();
}

void ParamListToParamList::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol ParamListToParamList::getLHSNonTerminal() const {
  return nonTerminal(NTID_ParamList);
}

std::vector<Symbol> ParamListToParamList::getProducts() const {
  return {nonTerminal(NTID_RValue), terminal(TID_ArgumentSeparator),
          nonTerminal(NTID_ParamList)};
}

ParamListToParam::ParamListToParam(const Lexer &LexImpl,
                                   ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void ParamListToParam::parse(TokenIterator ItToken) {
  Builder->createParameterList();
  Builder->createParameter();
}

void ParamListToParam::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol ParamListToParam::getLHSNonTerminal() const {
  return nonTerminal(NTID_ParamList);
}

std::vector<Symbol> ParamListToParam::getProducts() const {
  return {nonTerminal(NTID_RValue)};
}

TopLevelExpressionToCompoundExpression::TopLevelExpressionToCompoundExpression(
    const Lexer &LexImpl, ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void TopLevelExpressionToCompoundExpression::parse(TokenIterator ItToken) {
  Builder->createCompoundExpression(
      Lex->getTokenLexeme(TID_ExpressionSeparator));
}

void TopLevelExpressionToCompoundExpression::produceError(
    CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol TopLevelExpressionToCompoundExpression::getLHSNonTerminal() const {
  return nonTerminal(NTID_TopLevelExpression);
}

std::vector<Symbol>
TopLevelExpressionToCompoundExpression::getProducts() const {
  return {nonTerminal(NTID_CompoundExpression)};
}

CompoundExpressionToExpressionSequence::CompoundExpressionToExpressionSequence(
    const Lexer &LexImpl, ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void CompoundExpressionToExpressionSequence::parse(TokenIterator ItToken) {}

void CompoundExpressionToExpressionSequence::produceError(
    CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol CompoundExpressionToExpressionSequence::getLHSNonTerminal() const {
  return nonTerminal(NTID_CompoundExpression);
}

std::vector<Symbol>
CompoundExpressionToExpressionSequence::getProducts() const {
  return {nonTerminal(NTID_Expression), terminal(TID_ExpressionSeparator),
          nonTerminal(NTID_CompoundExpression)};
}

CompoundExpressionToSingleExpression::CompoundExpressionToSingleExpression(
    const Lexer &LexImpl, ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void CompoundExpressionToSingleExpression::parse(TokenIterator ItToken) {}

void CompoundExpressionToSingleExpression::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol CompoundExpressionToSingleExpression::getLHSNonTerminal() const {
  return nonTerminal(NTID_CompoundExpression);
}

std::vector<Symbol> CompoundExpressionToSingleExpression::getProducts() const {
  return {nonTerminal(NTID_Expression), terminal(TID_ExpressionSeparator)};
}

ExpressionToAssignment::ExpressionToAssignment(const Lexer &LexImpl,
                                               ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void ExpressionToAssignment::parse(TokenIterator ItToken) {
  Builder->createAssignmentExpression(Lex->getTokenLexeme(TID_Assignment));
}

void ExpressionToAssignment::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol ExpressionToAssignment::getLHSNonTerminal() const {
  return nonTerminal(NTID_Expression);
}

std::vector<Symbol> ExpressionToAssignment::getProducts() const {
  return {nonTerminal(NTID_LValue), terminal(TID_Assignment),
          nonTerminal(NTID_RValue)};
}

ExpressionToRValue::ExpressionToRValue(const Lexer &LexImpl,
                                       ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void ExpressionToRValue::parse(TokenIterator ItToken) {}

void ExpressionToRValue::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol ExpressionToRValue::getLHSNonTerminal() const {
  return nonTerminal(NTID_Expression);
}

std::vector<Symbol> ExpressionToRValue::getProducts() const {
  return {nonTerminal(NTID_RValue)};
}

RValueToCall::RValueToCall(const Lexer &LexImpl, ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void RValueToCall::parse(TokenIterator ItToken) {
  Builder->createFunctionCall(*ItToken);
}

void RValueToCall::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol RValueToCall::getLHSNonTerminal() const {
  return nonTerminal(NTID_RValue);
}

std::vector<Symbol> RValueToCall::getProducts() const {
  return {terminal(TID_Identifier), terminal(TID_OpenParantheses),
          nonTerminal(NTID_ParamList), terminal(TID_CloseParantheses)};
}

RValueToLValue::RValueToLValue(const Lexer &LexImpl,
                               ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void RValueToLValue::parse(TokenIterator ItToken) {}

void RValueToLValue::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol RValueToLValue::getLHSNonTerminal() const {
  return nonTerminal(NTID_RValue);
}

std::vector<Symbol> RValueToLValue::getProducts() const {
  return {nonTerminal(NTID_LValue)};
}

RValueToIntegralNumber::RValueToIntegralNumber(const Lexer &LexImpl,
                                               ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void RValueToIntegralNumber::parse(TokenIterator ItToken) {
  Builder->createIntegralNumber(*ItToken);
}

void RValueToIntegralNumber::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol RValueToIntegralNumber::getLHSNonTerminal() const {
  return nonTerminal(NTID_RValue);
}

std::vector<Symbol> RValueToIntegralNumber::getProducts() const {
  return {terminal(TID_IntegralNumber)};
}

LValueToIdentifier::LValueToIdentifier(const Lexer &LexImpl,
                                       ASTBuilder &ASTBulderObject)
    : Lex(&LexImpl), Builder(&ASTBulderObject) {}

void LValueToIdentifier::parse(TokenIterator ItToken) {
  Builder->createVariable(*ItToken);
}

void LValueToIdentifier::produceError(CFGParserError Error) {
  printError(Error, *Lex);
}

Symbol LValueToIdentifier::getLHSNonTerminal() const {
  return Symbol(NTID_LValue, false);
}

std::vector<Symbol> LValueToIdentifier::getProducts() const {
  return {Symbol(TID_Identifier)};
}
