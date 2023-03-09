#include "cbc/Tokenizers.h"

#include "cbc/Driver.h"
#include "common/Metadata.h"
#include <string_view>
#include <utility>

namespace cb {

std::pair<common::Token, std::string>
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
    return std::make_pair(common::Token(), "");
  }
  auto Result = common::Token(TokenID::Identifier, TokenPriority::Normal);
  return std::make_pair(Result, std::string(Word.cbegin(), It));
}

std::pair<common::Token, std::string>
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
    return std::make_pair(common::Token(), "");
  }
  auto Result = common::Token(TokenID::IntegralNumber, TokenPriority::Normal);
  return std::make_pair(Result, std::string(Word.cbegin(), It));
}

KeywordTokenizer::KeywordTokenizer(TokenID TokID, std::string Keyword)
    : ID(TokID), KeywordString(std::move(Keyword)) {}

std::pair<common::Token, std::string>
KeywordTokenizer::tokenize(std::string_view Word) {
  if (Word.find(KeywordString)) {
    return std::make_pair(common::Token(), "");
  }
  auto Result = common::Token(ID, TokenPriority::High);
  return std::make_pair(Result, KeywordString);
}

} // namespace cb
