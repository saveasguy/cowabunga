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
#include <unistd.h>
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

class ParenthesizedExpressionParser;

class PrimaryExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Optional<IntegralNumberParser, VariableParser,
                      ParenthesizedExpressionParser> {
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

class BinaryExpressionParser;

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

class BinaryExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<
          PrimaryExpressionParser,
          TerminalInRange<BinaryOperatorsRangeBegin, BinaryOperatorsRangeEnd>,
          GeneralExpressionParser> {
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
    auto BinaryOperatorToken =
        getLowestPrecedenceBinaryOperatorToken(ItMatchedBegin, ItMatchedEnd);
    if (BinaryOperatorToken == ItMatchedEnd) {
      GeneralExpressionParser Parser;
      return Parser.parse(ItMatchedBegin, ItMatchedEnd);
    }
    auto LHS = buildAST(ItMatchedBegin, BinaryOperatorToken);
    auto RHS = buildAST(BinaryOperatorToken + 1, ItMatchedEnd);
    if (!LHS || !RHS) {
      return nullptr;
    }
    return std::make_unique<BinaryExpressionASTNode>(
        *BinaryOperatorToken, std::move(LHS), std::move(RHS));
  }

  static TokenIterator
  getLowestPrecedenceBinaryOperatorToken(TokenIterator ItMatchedBegin,
                                         TokenIterator ItMatchedEnd) {
    auto &OperatorPrecedenceTable = operatorPrecedenceTable();
    auto &OperatorAssociativityTable = operatorAssociativityTable();
    int LowestPrecedence = std::numeric_limits<int>::max();
    OperatorAssociativity LowestPrecedenceTokenAssociativity = Left;
    TokenIterator LowestPrecedenceToken = ItMatchedEnd;
    int ParenthesesDepth = 0;
    for (auto It = ItMatchedBegin; It != ItMatchedEnd; ++It) {
      if (It->id() == OpenParantheses) {
        ++ParenthesesDepth;
      } else if (It->id() == CloseParantheses) {
        --ParenthesesDepth;
      }
      if (ParenthesesDepth || !isBinaryOperator(*It)) {
        continue;
      }
      auto TokOperatorID = static_cast<OperatorID>(It->id());
      int Precedence = OperatorPrecedenceTable.at(TokOperatorID);
      auto Associativity = OperatorAssociativityTable.at(TokOperatorID);
      if (!isLowerPrecedence(Precedence, LowestPrecedence, Associativity)) {
        continue;
      }
      LowestPrecedence = Precedence;
      LowestPrecedenceTokenAssociativity = Associativity;
      LowestPrecedenceToken = It;
    }
    return LowestPrecedenceToken;
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

class ParenthesizedExpressionParser final
    : public IParser<std::unique_ptr<IASTNode>>,
      public Sequence<Terminal<OpenParantheses>, GeneralExpressionParser,
                      Terminal<CloseParantheses>> {
public:
  std::unique_ptr<IASTNode> parse(TokenIterator ItBegin,
                                  TokenIterator ItEnd) override {
    auto ItMatchedEnd = match(ItBegin, ItEnd);
    if (ItMatchedEnd == ItBegin) {
      return nullptr;
    }
    assert(ItBegin->id() == OpenParantheses && "Open parentheses expected");
    assert((ItMatchedEnd - 1)->id() == CloseParantheses &&
           "Close parentheses expected");
    GeneralExpressionParser InternalExpressionParser;
    std::unique_ptr<IASTNode> InternalExpression =
        InternalExpressionParser.parse(ItBegin + 1, ItMatchedEnd - 1);
    return std::make_unique<ParenthesizedExpressionASTNode>(
        *ItBegin, std::move(InternalExpression), *(ItMatchedEnd - 1));
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
