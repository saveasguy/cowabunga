#include "cowabunga/CBC/ASTPasses.h"
#include "cowabunga/CBC/Definitions.h"
#include "cowabunga/CBC/Parsers.h"
#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Lexer/Lexer.h"

#include <iostream>
#include <sstream>

using namespace cb;

int main(int argc, char **argv) {
  Lexer Lex;
  Lex.addTokenizer(IntegralNumberTokenizer())
      .addTokenizer(KeywordTokenizer(Addition, "+"))
      .addTokenizer(KeywordTokenizer(Substraction, "-"))
      .addTokenizer(KeywordTokenizer(Multiplication, "*"));
  std::stringstream Script;
  Script << "123 + 43 * 54 + 32 * 34";
  auto Tokens = Lex.tokenize(Script, "stringstream");
  BinaryExpressionParser Parser;
  auto AST = Parser.parse(Tokens.cbegin(), Tokens.cend());
  if (!AST) {
    std::cerr << "Failed to build AST." << std::endl;
    return 2;
  }
  ASTPrinter Printer(std::cout);
  AST->acceptASTPass(Printer);
  std::cout << std::endl;
  return 0;
}
