#include "cowabunga/CBC/Tokenizers.h"

#include <optional>
#include <string_view>
#include <utility>

using namespace cb;

std::pair<std::optional<Token>, size_t>
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
    return std::make_pair(std::nullopt, 0);
  }
  auto Result = Token(TID_Identifier);
  return std::make_pair(Result, It - Word.cbegin());
}

int IdentifierTokenizer::getTokenID() const {
  return TID_Identifier;
}

std::string IdentifierTokenizer::getLexemeString() const {
  return "identifier";
}

std::pair<std::optional<Token>, size_t>
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
    return std::make_pair(std::nullopt, 0);
  }
  auto Result = Token(TID_IntegralNumber);
  return std::make_pair(Result, It - Word.cbegin());
}

int IntegralNumberTokenizer::getTokenID() const {
  return TID_IntegralNumber;
}

std::string IntegralNumberTokenizer::getLexemeString() const {
  return "integer";
}

KeywordTokenizer::KeywordTokenizer(TokenID TokID, std::string Keyword)
    : ID(TokID), KeywordString(std::move(Keyword)) {}

std::pair<std::optional<Token>, size_t>
KeywordTokenizer::tokenize(std::string_view Word) {
  if (Word.find(KeywordString)) {
    return std::make_pair(std::nullopt, 0);
  }
  auto Result = Token(ID);
  return std::make_pair(Result, KeywordString.length());
}

int KeywordTokenizer::getTokenID() const {
  return ID;
}

std::string KeywordTokenizer::getLexemeString() const {
  return KeywordString;
}
