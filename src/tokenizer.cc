#include "tokenizer.h"

#include <cctype>
#include <memory>
#include <string>
#include <string_view>

#include "driver.h"
#include "token.h"

namespace kaleidoc {

// EOF tokenizer

Token EofTokenizer::Tokenize(std::string_view word) const {
  if (!word.empty()) { return Token{}; }
  return Token{TokenId::kEof, "EOF", TokenPriority::kHigh};
}

std::unique_ptr<Tokenizer> EofTokenizer::Clone() const {
  return std::make_unique<EofTokenizer>(*this);
}

// Identifier tokenizer

Token IdentifierTokenizer::Tokenize(std::string_view word) const {
  const auto *it = word.cbegin();
  bool match = it != word.cend() && (std::isalpha(*it) != 0 || *it == '_');
  while (match && it != word.cend()) {
    match = std::isalnum(*it) != 0 || *it == '_';
    if (match) {
      ++it;
    }
  }
  if (it == word.cbegin()) { return Token{}; }
  std::string matched_string{word.cbegin(), it};
  return Token{TokenId::kIdentifier, matched_string, TokenPriority::kNormal};
}

std::unique_ptr<Tokenizer> IdentifierTokenizer::Clone() const {
  return std::make_unique<IdentifierTokenizer>(*this);
}

// Keyword tokenizer

KeywordTokenizer::KeywordTokenizer(TokenId token_id, std::string keyword)
    : token_id_{token_id}, stringified_keyword_{std::move(keyword)} {}

Token KeywordTokenizer::Tokenize(std::string_view word) const {
  if (word.find(stringified_keyword_) != 0) { return Token{}; }
  return Token{token_id_, stringified_keyword_, TokenPriority::kHigh};
}

std::unique_ptr<Tokenizer> KeywordTokenizer::Clone() const {
  return std::make_unique<KeywordTokenizer>(*this);
}

// Integral number tokenizer

Token IntegralNumberTokenizer::Tokenize(std::string_view word) const {
  bool match = true;
  const auto *it = word.cbegin();
  while (match && it != word.cend()) {
    match = std::isdigit(*it) != 0;
    if (match) {
      ++it;
    }
  }
  if (it == word.cbegin()) { return Token{}; }
  std::string matched_string{word.cbegin(), it};
  return Token{TokenId::kIntegralNumber, matched_string,
               TokenPriority::kNormal};
}

std::unique_ptr<Tokenizer> IntegralNumberTokenizer::Clone() const {
  return std::make_unique<IntegralNumberTokenizer>(*this);
}

}  // namespace kaleidoc
