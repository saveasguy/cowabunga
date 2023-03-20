#include "cowabunga/CBC/Parsers.h"

using namespace cb;

std::unique_ptr<IASTNode> VariableParser::parse(TokenIterator ItBegin,
                                                TokenIterator ItEnd) {
  auto ItMatchedEnd = match(ItBegin, ItEnd);
  if (ItMatchedEnd == ItBegin) {
    return nullptr;
  }
  return std::make_unique<VariableASTNode>(*ItBegin);
}

std::unique_ptr<IASTNode> IntegralNumberParser::parse(TokenIterator ItBegin,
                                                      TokenIterator ItEnd) {
  auto ItMatchedEnd = match(ItBegin, ItEnd);
  if (ItMatchedEnd == ItBegin) {
    return nullptr;
  }
  return std::make_unique<IntegralNumberASTNode>(*ItBegin);
}

std::unique_ptr<IASTNode> PrimaryExpressionParser::parse(TokenIterator ItBegin,
                                                         TokenIterator ItEnd) {
  auto ItMatchedEnd = match(ItBegin, ItEnd);
  if (ItMatchedEnd == ItBegin) {
    return nullptr;
  }
  std::unique_ptr<IASTNode> AST;
  callbackOnMatched(
      ItBegin, ItMatchedEnd,
      [&AST](IParser<std::unique_ptr<IASTNode>> &Parser, TokenIterator ItBegin,
             TokenIterator ItEnd) { AST = Parser.parse(ItBegin, ItEnd); });
  return AST;
}

std::unique_ptr<IASTNode> GeneralExpressionParser::parse(TokenIterator ItBegin,
                                                         TokenIterator ItEnd) {
  auto ItMatchedEnd = match(ItBegin, ItEnd);
  if (ItMatchedEnd == ItBegin) {
    return nullptr;
  }
  std::unique_ptr<IASTNode> AST;
  callbackOnMatched(
      ItBegin, ItMatchedEnd,
      [&AST](IParser<std::unique_ptr<IASTNode>> &Parser, TokenIterator ItBegin,
             TokenIterator ItEnd) { AST = Parser.parse(ItBegin, ItEnd); });
  return AST;
}

std::unique_ptr<IASTNode> BinaryExpressionParser::parse(TokenIterator ItBegin,
                                                        TokenIterator ItEnd) {
  auto ItMatchedEnd = match(ItBegin, ItEnd);
  if (ItMatchedEnd == ItBegin) {
    return nullptr;
  }
  return buildAST(ItBegin, ItMatchedEnd);
}

std::unique_ptr<IASTNode>
BinaryExpressionParser::buildAST(TokenIterator ItMatchedBegin,
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

TokenIterator BinaryExpressionParser::getLowestPrecedenceBinaryOperatorToken(
    TokenIterator ItMatchedBegin, TokenIterator ItMatchedEnd) {
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

bool BinaryExpressionParser::isBinaryOperator(const Token &Tok) {
  return Tok.id() > BinaryOperatorsRangeBegin &&
         Tok.id() < BinaryOperatorsRangeEnd;
}

bool BinaryExpressionParser::isLowerPrecedence(
    int Precedence, int ComparedToPrecedence,
    OperatorAssociativity Associativity) {
  if (Associativity == Left) {
    return Precedence < ComparedToPrecedence;
  }
  return Precedence <= ComparedToPrecedence;
}

std::unique_ptr<IASTNode>
ParenthesizedExpressionParser::parse(TokenIterator ItBegin,
                                     TokenIterator ItEnd) {
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

std::unique_ptr<IASTNode> ExpressionParser::parse(TokenIterator ItBegin,
                                                  TokenIterator ItEnd) {
  auto ItMatchedEnd = match(ItBegin, ItEnd);
  if (ItMatchedEnd == ItBegin) {
    return nullptr;
  }
  GeneralExpressionParser Parser;
  return Parser.parse(ItBegin, ItEnd);
}

std::unique_ptr<IASTNode> CompoundExpressionHelper::parse(TokenIterator ItBegin,
                                                          TokenIterator ItEnd) {
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
  assert(Expressions[0] && Expressions[1] && "Subsuquent expressions expected");
  ExpressionParser BasicExprParser;
  auto ExpressionSeparatorToken =
      BasicExprParser.match(ItBegin, ItMatchedEnd) - 1;
  return std::make_unique<CompoundExpressionASTNode>(*ExpressionSeparatorToken,
                                                     std::move(Expressions[0]),
                                                     std::move(Expressions[1]));
}

std::unique_ptr<IASTNode> CompoundExpressionParser::parse(TokenIterator ItBegin,
                                                          TokenIterator ItEnd) {
  auto ItMatchedEnd = match(ItBegin, ItEnd);
  if (ItMatchedEnd == ItBegin) {
    return nullptr;
  }
  std::unique_ptr<IASTNode> AST;
  callbackOnMatched(
      ItBegin, ItMatchedEnd,
      [&AST](IParser<std::unique_ptr<IASTNode>> &Parser, TokenIterator ItBegin,
             TokenIterator ItEnd) { AST = Parser.parse(ItBegin, ItEnd); });
  return AST;
}
