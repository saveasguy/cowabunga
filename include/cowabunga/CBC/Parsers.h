#ifndef COWABUNGA_CBC_PARSERS_H
#define COWABUNGA_CBC_PARSERS_H

#include "cowabunga/CBC/ASTNodes.h"
#include "cowabunga/CBC/Definitions.h"
#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Common/Metadata.h"
#include "cowabunga/Lexer/Token.h"
#include "cowabunga/Parser/IParser.h"
#include "cowabunga/Parser/Matchers.h"

#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <unistd.h>
#include <vector>

namespace cb {

class VariableParser final : public IParser<std::unique_ptr<IASTNode>>,
                             public Terminal<Identifier> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

class IntegralNumberParser final : public IParser<std::unique_ptr<IASTNode>>,
                                   public Terminal<IntegralNumber> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

class ParenthesizedExpressionParser;

class PrimaryExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Optional<IntegralNumberParser, VariableParser,
                      ParenthesizedExpressionParser> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

class BinaryExpressionParser;

class GeneralExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Optional<BinaryExpressionParser, PrimaryExpressionParser> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

class BinaryExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<
          PrimaryExpressionParser,
          TerminalInRange<BinaryOperatorsRangeBegin, BinaryOperatorsRangeEnd>,
          GeneralExpressionParser> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;

private:
  static std::unique_ptr<IASTNode> buildAST(TokenIterator ItMatchedBegin,
                                            TokenIterator ItMatchedEnd);

  static TokenIterator
  getLowestPrecedenceBinaryOperatorToken(TokenIterator ItMatchedBegin,
                                         TokenIterator ItMatchedEnd);

  static bool isBinaryOperator(const Token &Tok);

  static bool isLowerPrecedence(int Precedence, int ComparedToPrecedence,
                                OperatorAssociativity Associativity);
};

class ParenthesizedExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<Terminal<OpenParantheses>, GeneralExpressionParser,
                      Terminal<CloseParantheses>> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

class ExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<GeneralExpressionParser, Terminal<ExpressionSeparator>> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

class CompoundExpressionParser;

class CompoundExpressionHelper final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<ExpressionParser, CompoundExpressionParser> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

class CompoundExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Optional<ExpressionParser, CompoundExpressionHelper> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override;
};

} // namespace cb

#endif // COWABUNGA_CBC_PARSERS_H
