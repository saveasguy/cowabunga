#ifndef CBC_PARSERS_H
#define CBC_PARSERS_H

#include "Driver.h"
#include "Tokenizers.h"
#include "cbc/ASTNodes.h"
#include "common/Metadata.h"
#include "common/Token.h"
#include "parser/IParser.h"
#include "parser/Matchers.h"

#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

namespace cb {

using namespace parser;

class VariableParser final : public IParser<std::unique_ptr<IASTNode>>,
                             public Terminal<Identifier> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    return std::make_unique<VariableASTNode>(*ItBegin);
  }
};

class IntegralNumberParser final : public IParser<std::unique_ptr<IASTNode>>,
                                   public Terminal<IntegralNumber> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    return std::make_unique<IntegralNumberASTNode>(*ItBegin);
  }
};

class PrimaryExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Optional<IntegralNumberParser, VariableParser> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    std::unique_ptr<IASTNode> AST;
    callbackOnMatched(ItBegin, ItMatchedEnd,
                      [&AST](IParser<std::unique_ptr<IASTNode>> &Parser,
                             TokenIterator ItBegin, TokenIterator ItEnd) {
                        AST = Parser.parse(ItBegin, ItEnd);
                      });
    return AST;
  }
};

class BinaryRHSExpressionMatcher final
    : public Sequence<
          TerminalInRange<BinaryOperatorsRangeBegin, BinaryOperatorsRangeEnd>,
          Optional<
              PrimaryExpressionParser,
              Sequence<PrimaryExpressionParser, BinaryRHSExpressionMatcher>>> {
};

class BinaryExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<PrimaryExpressionParser, BinaryRHSExpressionMatcher> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    return buildAST(ItBegin, ItMatchedEnd);
  }

private:
  static std::unique_ptr<IASTNode> buildAST(TokenIterator ItMatchedBegin,
                                            TokenIterator ItMatchedEnd) {
    auto &OperatorPrecedenceTable = operatorPrecedenceTable();
    auto &OperatorAssociativityTable = operatorAssociativityTable();
    int TopLevelPrecedence = std::numeric_limits<int>::max();
    OperatorAssociativity TopLevelAssociativity = Left;
    TokenIterator TopLevelToken = ItMatchedEnd;
    for (auto It = ItMatchedBegin; It != ItMatchedEnd; ++It) {
      if (!isBinaryOperator(*It)) {
        continue;
      }
      auto TokOperatorID = static_cast<OperatorID>(It->id());
      int Precedence = OperatorPrecedenceTable.at(TokOperatorID);
      auto Associativity = OperatorAssociativityTable.at(TokOperatorID);
      if (!isLowerPrecedence(Precedence, TopLevelPrecedence, Associativity)) {
        continue;
      }
      TopLevelPrecedence = Precedence;
      TopLevelAssociativity = Associativity;
      TopLevelToken = It;
    }
    if (TopLevelToken == ItMatchedEnd) {
      PrimaryExpressionParser Parser;
      return Parser.parse(ItMatchedBegin, ItMatchedEnd);
    }
    auto LHS = buildAST(ItMatchedBegin, TopLevelToken);
    auto RHS = buildAST(TopLevelToken + 1, ItMatchedEnd);
    if (!LHS || !RHS) {
      return nullptr;
    }
    return std::make_unique<BinaryExpressionASTNode>(
        *TopLevelToken, std::move(LHS), std::move(RHS));
  }

  static bool isBinaryOperator(const common::Token &Tok) {
    return Tok.id() > BinaryOperatorsRangeBegin &&
           Tok.id() < BinaryOperatorsRangeEnd;
  }

  static bool isLowerPrecedence(int Precedence, int ComparedToPrecedence,
                                OperatorAssociativity Associativity) {
    if (Associativity == Left) {
      return Precedence < ComparedToPrecedence;
    }
    return Precedence <= ComparedToPrecedence;
  }
};

class GeneralExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Optional<BinaryExpressionParser, PrimaryExpressionParser> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    std::unique_ptr<IASTNode> AST;
    callbackOnMatched(ItBegin, ItMatchedEnd,
                      [&AST](IParser<std::unique_ptr<IASTNode>> &Parser,
                             TokenIterator ItBegin, TokenIterator ItEnd) {
                        AST = Parser.parse(ItBegin, ItEnd);
                      });
    return AST;
  }
};

class ExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<GeneralExpressionParser, Terminal<ExpressionSeparator>> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    GeneralExpressionParser Parser;
    return Parser.parse(ItBegin, ItEnd);
  }
};

class CompoundExpressionParser;

class CompoundExpressionHelper final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<ExpressionParser, CompoundExpressionParser> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    std::array<std::unique_ptr<IASTNode>, 2> Expressions;
    size_t Idx = 0;
    callbackOnMatched(
        ItBegin, ItMatchedEnd,
        [&Expressions, &Idx](IParser<std::unique_ptr<IASTNode>> &Parser,
                             TokenIterator ItBegin, TokenIterator ItEnd) {
          Expressions[Idx] = Parser.parse(ItBegin, ItEnd);
          ++Idx;
        });
    assert(Expressions[0] && Expressions[1] &&
           "Subsuquent expressions expected");
    ExpressionParser BasicExprParser;
    auto ExpressionSeparatorToken =
        BasicExprParser.match(ItBegin, ItMatchedEnd) - 1;
    return std::make_unique<CompoundExpressionASTNode>(
        *ExpressionSeparatorToken, std::move(Expressions[0]),
        std::move(Expressions[1]));
  }
};

class CompoundExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Optional<ExpressionParser, CompoundExpressionHelper> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    std::unique_ptr<IASTNode> AST;
    callbackOnMatched(ItBegin, ItMatchedEnd,
                      [&AST](IParser<std::unique_ptr<IASTNode>> &Parser,
                             TokenIterator ItBegin, TokenIterator ItEnd) {
                        AST = Parser.parse(ItBegin, ItEnd);
                      });
    return AST;
  }
};

} // namespace cb

#endif
