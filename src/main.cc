#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "driver.h"
#include "lexer.h"
#include "tokenizer.h"

int main(int argc, char **argv) {
  kaleidoc::FullTextLexer lexer{};
  lexer.AddTokenizer(std::make_unique<kaleidoc::EofTokenizer>())
      .AddTokenizer(std::make_unique<kaleidoc::IdentifierTokenizer>())
      .AddTokenizer(std::make_unique<kaleidoc::IntegralNumberTokenizer>())
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kSemicolon, ";"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kOperator, "="))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kOperator, "+"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kOperator, "-"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kDef, "def"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kExtern, "extern"));

  std::ifstream script;
  if (argc == 2) {
    script.open(argv[1]);
  } else {
    std::cerr << "No input files" << std::endl;
    exit(1);
  }
  auto tokens = lexer.ProduceTokens(script);
  for (const auto &token : tokens) {
    std::cout << token.id() << "(" << token.stringified() << ")" << std::endl;
  }
  return 0;
}