#include <fstream>
#include <iostream>
#include <memory>

#include "driver.h"
#include "lexeme_analyzer.h"
#include "lexer.h"

int main(int argc, char **argv) {
  kaleidoc::SymbolicLexer lexer{};
  lexer.AddAnalyzer(std::make_unique<kaleidoc::IdentifierAnalyzer>())
      .AddAnalyzer(std::make_unique<kaleidoc::IntegralNumberAnalyzer>())
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kAssignment, "="))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kPlus, "+"))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kMinus, "-"))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kDef, "def"))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kExtern, "extern"));

  std::istream *input = &std::cin;
  std::ifstream script;
  if (argc == 2) {
    script.open(argv[1]);
    input = &script;
  } else {
    std::cout << "Press Ctrl+C to leave REPL" << std::endl;
  }
  std::unique_ptr<kaleidoc::Token> token = nullptr;
  do {
    token = lexer.NextToken(*input);
    std::cout << token->GetTokenId();
    auto metadata = token->GetMetadata();
    if (metadata.find(kaleidoc::MetadataType::kValue) != metadata.end()) {
      std::cout << " (" <<metadata[kaleidoc::MetadataType::kValue] << ")";
    }
    std::cout << std::endl;
  } while (token->GetTokenId() != kaleidoc::TokenId::kEof);
  return 0;
}