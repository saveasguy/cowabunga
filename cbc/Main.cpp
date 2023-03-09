#include "cbc/Driver.h"
#include "cbc/Parsers.h"
#include "cbc/Tokenizers.h"
#include "lexer/Lexer.h"

#include <fstream>
#include <iostream>

using namespace cb;

int main(int argc, char **argv) {
  lexer::Lexer Lexer{};
  Lexer.addTokenizer(IntegralNumberTokenizer())
      .addTokenizer(IdentifierTokenizer())
      .addTokenizer(KeywordTokenizer(ExpressionSeparator, ";"))
      .addTokenizer(KeywordTokenizer(Assignment, "="))
      .addTokenizer(KeywordTokenizer(Plus, "+"))
      .addTokenizer(KeywordTokenizer(Minus, "-"))
      .addTokenizer(KeywordTokenizer(ShiftLeft, "<<"))
      .addTokenizer(KeywordTokenizer(ShiftRight, ">>"));

  std::ifstream Script;
  if (argc == 2) {
    Script.open(argv[1]);
  } else {
    std::cerr << "No input files" << std::endl;
    exit(1);
  }
  auto Tokens = Lexer.produceTokens(Script);
  if (!Tokens) {
    std::cerr << "Failed to parse input file" << std::endl;
    exit(1);
  }
  for (auto &Token : *Tokens) {
    std::cout << Token << std::endl;
  }
  ExpressionParser Parser;
  auto It = Parser.match(Tokens->cbegin(), Tokens->cend());
  bool Ok = (It == Tokens->cend());
  std::cout << Ok << std::endl;
  if (!Ok) {
    return 1;
  }
  auto AST = Parser.parse(Tokens->cbegin(), Tokens->cend());
  std::cout << *AST << std::endl;
  return 0;
}