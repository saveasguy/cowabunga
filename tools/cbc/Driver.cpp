#include "cowabunga/CBC/ASTPasses.h"
#include "cowabunga/CBC//Parsers.h"
#include "cowabunga/CBC//Tokenizers.h"
#include "cowabunga/CBC/Definitions.h"
#include "cowabunga/Lexer/Lexer.h"

#include <fstream>
#include <iostream>

using namespace cb;

int main(int argc, char **argv) {
  Lexer Lex;
  Lex.addTokenizer(IntegralNumberTokenizer())
      .addTokenizer(IdentifierTokenizer())
      .addTokenizer(KeywordTokenizer(ExpressionSeparator, ";"))
      .addTokenizer(KeywordTokenizer(Assignment, "="))
      .addTokenizer(KeywordTokenizer(Addition, "+"))
      .addTokenizer(KeywordTokenizer(Substraction, "-"))
      .addTokenizer(KeywordTokenizer(ShiftLeft, "<<"))
      .addTokenizer(KeywordTokenizer(ShiftRight, ">>"))
      .addTokenizer(KeywordTokenizer(OpenParantheses, "("))
      .addTokenizer(KeywordTokenizer(CloseParantheses, ")"))
      .addTokenizer(KeywordTokenizer(Multiplication, "*"));

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
  for (auto &Token : Tokens) {
    std::cout << Token << std::endl;
  }
  CompoundExpressionParser Parser;
  auto It = Parser.match(Tokens.cbegin(), Tokens.cend());
  bool Ok = (It == Tokens.cend());
  std::cout << Ok << std::endl;
  if (!Ok) {
    std::cerr << "Failed to parse input file." << std::endl;
    return 4;
  }
  auto AST = Parser.parse(Tokens.cbegin(), Tokens.cend());
  if (!AST) {
    std::cerr << "Failed to build AST." << std::endl;
    return 4;
  }
  ASTPrinter Printer(std::cout);
  AST->acceptASTPass(Printer);
  std::cout << std::endl;
  return 0;
}
