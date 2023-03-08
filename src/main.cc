#include <fstream>
#include <iostream>
#include <memory>
#include <string_view>

#include "driver.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char **argv) {
  kaleidoc::FullTextLexer lexer{};
  lexer.AddTokenizer(std::make_unique<kaleidoc::EofTokenizer>())
      .AddTokenizer(std::make_unique<kaleidoc::IdentifierTokenizer>())
      .AddTokenizer(std::make_unique<kaleidoc::IntegralNumberTokenizer>())
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kArgumentSeparator, ","))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kExpressionSeparator, ";"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kAssignment, "="))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kPlus, "+"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kMinus, "-"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kShiftLeft, "<<"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kShiftRight, ">>"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kDefinition, "def"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kExternalDeclaration, "extern"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kBodyBegin, "begin"))
      .AddTokenizer(std::make_unique<kaleidoc::KeywordTokenizer>(
          kaleidoc::TokenId::kBodyEnd, "end"));

  std::ifstream script;
  if (argc == 2) {
    script.open(argv[1]);
  } else {
    std::cerr << "No input files" << std::endl;
    exit(1);
  }
  auto tokens = lexer.ProduceTokens(script);
  for (const auto &token : tokens) {
    std::cout << token << std::endl;
  }
  std::cout << "-----------------------------------------" << std::endl;
  auto variable_builder = std::make_unique<kaleidoc::VariableAstBuilder>();
  auto int_builder = std::make_unique<kaleidoc::IntegralNumberAstBuilder>();
  auto primary_builder =
      std::make_unique<kaleidoc::PrimaryExpressionAstBuilder>();
  primary_builder->AddBuilder(variable_builder.get());
  primary_builder->AddBuilder(int_builder.get());
  auto binary_builder = std::make_unique<kaleidoc::BinaryExpressionAstBuilder>();
  binary_builder->SetPrimaryBuilder(primary_builder.get());
  kaleidoc::ExpressionSequenceAstBuilder builder;
  builder.AddExpressionBuilder(primary_builder.get());
  builder.AddExpressionBuilder(binary_builder.get());
  auto ast = builder.Build(tokens.cbegin(), tokens.cend()).first;
  std::cout << *ast << std::endl;
  return 0;
}