#include <fstream>
#include <iostream>
#include <memory>

#include "driver.h"
#include "lexeme_analyzer.h"
#include "lexer.h"

int main(int argc, char **argv) {
  kaleidoc::SymbolicLexer lexer{};
  lexer.AddAnalyzer(std::make_unique<kaleidoc::IdentifierAnalyzer>())
      .AddAnalyzer(std::make_unique<kaleidoc::NumberAnalyzer>())
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kAssignmentId, "="))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kPlusId, "+"))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kMinusId, "-"))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kDefId, "def"))
      .AddAnalyzer(std::make_unique<kaleidoc::KeywordAnalyzer>(
          kaleidoc::TokenId::kExternId, "extern"));

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
    std::cout << token->GetValue() << "(" << token->GetToken() << ")"
              << std::endl;
  } while (token->GetToken() != kaleidoc::kEofId);
  return 0;
}