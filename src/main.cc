#include <fstream>
#include <iostream>

#include "driver.h"
#include "lexeme_analyzer.h"
#include "lexer.h"

int main(int argc, char **argv) {
  kaleidoc::Lexer *lexer = new kaleidoc::SymbolicLexer{};
  lexer->AddAnalyzer(new kaleidoc::IdentifierAnalyzer)
      ->AddAnalyzer(new kaleidoc::NumberAnalyzer)
      ->AddAnalyzer(new kaleidoc::BinaryOperatorAnalyzer{ "+" })
      ->AddAnalyzer(new kaleidoc::BinaryOperatorAnalyzer{ "-" })
      ->AddAnalyzer(new kaleidoc::BinaryOperatorAnalyzer{ "=" })
      ->AddAnalyzer(new kaleidoc::BinaryOperatorAnalyzer{ "+=" })
      ->AddAnalyzer(new kaleidoc::BinaryOperatorAnalyzer{ "-=" })
      ->AddAnalyzer(
          new kaleidoc::KeywordAnalyzer{ kaleidoc::TokenId::kDefId, "def" })
      ->AddAnalyzer(new kaleidoc::KeywordAnalyzer{ kaleidoc::TokenId::kExternId,
                                                   "extern" });

  std::istream *input = &std::cin;
  std::ifstream script;
  if (argc == 2) {
    
    script.open(argv[1]);
    input = &script;
  } else {
    std::cout << "Press Ctrl+C to leave REPL" << std::endl;
  }
  kaleidoc::Token *token = nullptr;
    do {
      delete token;
      token = lexer->NextToken(*input);
      std::cout << token->GetValue() << "(" << token->GetToken() << ")" << std::endl;
    } while (token->GetToken() != kaleidoc::kEofId);
    delete token;
    delete lexer;
  return 0;
}