#include "cowabunga/CBC//Parsers.h"
#include "cowabunga/CBC//Tokenizers.h"
#include "cowabunga/CBC/ASTBuilder.h"
#include "cowabunga/CBC/ASTPasses.h"
#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/CFGParser.h"
#include "cowabunga/Parser/Symbol.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace cb;

int main(int argc, char **argv) {
  Lexer Lex;
  Lex.addTokenizer(IdentifierTokenizer())
      .addTokenizer(IntegralNumberTokenizer())
      .addTokenizer(KeywordTokenizer(TID_ExpressionSeparator, ";"))
      .addTokenizer(KeywordTokenizer(TID_Assignment, "="))
      .addTokenizer(KeywordTokenizer(TID_OpenParantheses, "("))
      .addTokenizer(KeywordTokenizer(TID_CloseParantheses, ")"))
      .addTokenizer(KeywordTokenizer(TID_ArgumentSeparator, ","));

  std::ifstream Script;
  if (argc == 2) {
    Script.open(argv[1]);
  } else {
    std::cerr << "No input files." << std::endl;
    return 1;
  }
  if (!Script) {
    std::cerr << "File not found." << std::endl;
    return 2;
  }
  auto Tokens = Lex.tokenize(Script, argv[1]);

  ASTBuilder Builder;
  CFGParser Parser(nonTerminal(NTID_TopLevelExpression));
  Parser.addCFGRule(LValueToIdentifier(Lex, Builder))
      .addCFGRule(RValueToLValue(Lex, Builder))
      .addCFGRule(RValueToIntegralNumber(Lex, Builder))
      .addCFGRule(ExpressionToRValue(Lex, Builder))
      .addCFGRule(CompoundExpressionToSingleExpression(Lex, Builder))
      .addCFGRule(CompoundExpressionToExpressionSequence(Lex, Builder))
      .addCFGRule(TopLevelExpressionToCompoundExpression(Lex, Builder))
      .addCFGRule(ExpressionToAssignment(Lex, Builder))
      .addCFGRule(RValueToCall(Lex, Builder))
      .addCFGRule(ParamListToParam(Lex, Builder))
      .addCFGRule(ParamListToParamList(Lex, Builder));

  Parser.parse(Tokens.begin(), Tokens.end());
  auto AST = Builder.release();
  ASTCodeGen CodeGen;
  AST->acceptASTPass(CodeGen);
  return CodeGen.compile();
}
