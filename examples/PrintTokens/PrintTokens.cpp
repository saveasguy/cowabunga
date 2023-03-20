#include "cowabunga/CBC/Definitions.h"
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
  auto Tokens = Lex.produceTokens(Script);
  if (!Tokens) {
    std::cerr << "Failed to parse input file." << std::endl;
    return 1;
  }
  for (const auto &Tok : *Tokens) {
    std::cout << Tok << std::endl;
  }
  return 0;
}
