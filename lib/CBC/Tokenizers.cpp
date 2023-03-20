#include "cowabunga/CBC/Tokenizers.h"

#include "cowabunga/CBC/Definitions.h"
#include "cowabunga/Common/Metadata.h"

#include <string_view>
#include <utility>

using namespace cb;

std::pair<Token, std::string>
IdentifierTokenizer::tokenize(std::string_view Word) {
  auto *It = Word.cbegin();
  bool Match = It != Word.cend() && (std::isalpha(*It) || *It == '_');
  while (Match && It != Word.cend()) {
    Match = std::isalnum(*It) || *It == '_';
    if (Match) {
      ++It;
    }
  }
  if (It == Word.cbegin()) {
    return std::make_pair(Token(), "");
  }
  auto Result = Token(TokenID::Identifier, TokenPriority::Normal);
  return std::make_pair(Result, std::string(Word.cbegin(), It));
}

std::pair<Token, std::string>
IntegralNumberTokenizer::tokenize(std::string_view Word) {
  bool Match = true;
  auto *It = Word.cbegin();
  while (Match && It != Word.cend()) {
    Match = std::isdigit(*It);
    if (Match) {
      ++It;
    }
  }
  if (It == Word.cbegin()) {
    return std::make_pair(Token(), "");
  }
  auto Result = Token(TokenID::IntegralNumber, TokenPriority::Normal);
  return std::make_pair(Result, std::string(Word.cbegin(), It));
}

KeywordTokenizer::KeywordTokenizer(TokenID TokID, std::string Keyword)
    : ID(TokID), KeywordString(std::move(Keyword)) {}

std::pair<Token, std::string>
KeywordTokenizer::tokenize(std::string_view Word) {
  if (Word.find(KeywordString)) {
    return std::make_pair(Token(), "");
  }
  auto Result = Token(ID, TokenPriority::High);
  return std::make_pair(Result, KeywordString);
}
