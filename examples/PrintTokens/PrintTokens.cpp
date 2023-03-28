#include "cowabunga/CBC/Tokenizers.h"
#include "cowabunga/Lexer/Lexer.h"

#include <iostream>
#include <sstream>

using namespace cb;

int main(int argc, char **argv) {
  Lexer Lex;
  Lex.addTokenizer(IdentifierTokenizer())
      .addTokenizer(IntegralNumberTokenizer())
      .addTokenizer(KeywordTokenizer(TID_ExpressionSeparator, ";"))
      .addTokenizer(KeywordTokenizer(TID_Assignment, "="));
  std::stringstream Script;
  Script << "a = 13; b = 4;";
  auto Tokens = Lex.tokenize(Script, "stringstream");
  for (const auto &Tok : Tokens) {
    std::cout << Tok << std::endl;
  }
  return 0;
}
