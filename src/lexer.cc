#include "lexer.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <istream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "driver.h"

namespace kaleidoc {

// --- EXCEPTIONS ---

UnknownLexemeException::UnknownLexemeException(std::string msg)
    : msg_{std::move(msg)} {}

const char *UnknownLexemeException::what() const noexcept {
  return msg_.c_str();
}

// --- LEXERS ---

// FULL TEXT LEXER

FullTextLexer::FullTextLexer(const FullTextLexer &rhs) {
  for (const auto &analyzer : rhs.tokenizers_) {
    tokenizers_.push_back(std::unique_ptr<Tokenizer>{analyzer->Clone()});
  }
}

FullTextLexer &FullTextLexer::operator=(const FullTextLexer &rhs) {
  tokenizers_.clear();
  for (const auto &analyzer : rhs.tokenizers_) {
    tokenizers_.push_back(std::unique_ptr<Tokenizer>{analyzer->Clone()});
  }
  return *this;
}

Lexer &FullTextLexer::AddTokenizer(std::unique_ptr<Tokenizer> tokenizer) {
  tokenizers_.push_back(std::move(tokenizer));
  return *this;
}

std::vector<Token> FullTextLexer::ProduceTokens(std::istream &input) const {
  std::vector<Token> tokens;
  std::string word;
  std::string_view word_view;
  do {
    if (!word_view.empty()) {
      auto offset =
          tokens.back().metadata().at(MetadataType::kStringified).length();
      word_view.remove_prefix(offset);
    }
    if (word_view.empty()) {
      word.clear();
      input >> word;
      word_view = std::string_view{word.c_str()};
    }
    Token token = FindBestToken(word_view);
    if (!token) {
      std::stringstream str;
      str << "Unknown lexeme: "
          << std::string{word_view.cbegin(), word_view.cend()};
      throw UnknownLexemeException{str.str()};
    }
    tokens.push_back(std::move(token));
  } while (input);
  return tokens;
}

Token FullTextLexer::FindBestToken(std::string_view word) const {
  Token best_token;
  for (const auto &tokenizer : tokenizers_) {
    Token cur_token = tokenizer->Tokenize(word);
    if (cur_token > best_token) { best_token = cur_token; }
  }
  return best_token;
}

// --- TOKENIZERS ---

// EOF TOKENIZER

Token EofTokenizer::Tokenize(std::string_view word) const {
  if (!word.empty()) { return Token{}; }
  return Token{TokenId::kEof, "EOF", TokenPriority::kHigh};
}

std::unique_ptr<Tokenizer> EofTokenizer::Clone() const {
  return std::make_unique<EofTokenizer>(*this);
}

// IDENTIFIER TOKENIZER

Token IdentifierTokenizer::Tokenize(std::string_view word) const {
  const auto *it = word.cbegin();
  bool match = it != word.cend() && (std::isalpha(*it) || *it == '_');
  while (match && it != word.cend()) {
    match = std::isalnum(*it) || *it == '_';
    if (match) { ++it; }
  }
  if (it == word.cbegin()) { return Token{}; }
  std::string matched_string{word.cbegin(), it};
  return Token{TokenId::kIdentifier, matched_string, TokenPriority::kNormal};
}

std::unique_ptr<Tokenizer> IdentifierTokenizer::Clone() const {
  return std::make_unique<IdentifierTokenizer>(*this);
}

// KEYWORD TOKENIZER

KeywordTokenizer::KeywordTokenizer(TokenId token_id, std::string keyword)
    : token_id_{token_id}, stringified_keyword_{std::move(keyword)} {}

Token KeywordTokenizer::Tokenize(std::string_view word) const {
  if (word.find(stringified_keyword_)) { return Token{}; }
  return Token{token_id_, stringified_keyword_, TokenPriority::kHigh};
}

std::unique_ptr<Tokenizer> KeywordTokenizer::Clone() const {
  return std::make_unique<KeywordTokenizer>(*this);
}

// INTEGRAL NUMBER TOKENIZER

Token IntegralNumberTokenizer::Tokenize(std::string_view word) const {
  bool match = true;
  const auto *it = word.cbegin();
  while (match && it != word.cend()) {
    match = std::isdigit(*it);
    if (match) { ++it; }
  }
  if (it == word.cbegin()) { return Token{}; }
  std::string matched_string{word.cbegin(), it};
  return Token{TokenId::kIntegralNumber, matched_string,
               TokenPriority::kNormal};
}

std::unique_ptr<Tokenizer> IntegralNumberTokenizer::Clone() const {
  return std::make_unique<IntegralNumberTokenizer>(*this);
}

// --- TOKEN ---

Token::Token() : priority_{TokenPriority::kUnmatched} {}

Token::Token(TokenId token_id, std::string matched_string,
             TokenPriority token_priority)
    : id_{token_id}, priority_{token_priority} {
  metadata_[MetadataType::kStringified] = std::move(matched_string);
}

void Token::AddMetadata(MetadataType type, std::string value) {
  metadata_[type] = std::move(value);
}

bool Token::Equals(const Token &rhs) const {
  auto stringified_it = metadata_.find(MetadataType::kStringified);
  auto rhs_stringified_it = rhs.metadata_.find(MetadataType::kStringified);
  bool stringified_equal = false;
  if (stringified_it != metadata_.end() &&
      rhs_stringified_it != rhs.metadata_.end()) {
    stringified_equal = (*stringified_it == *rhs_stringified_it);
  }
  return stringified_equal && priority_ == rhs.priority_;
}

bool Token::Less(const Token &rhs) const {
  auto stringified_it = metadata_.find(MetadataType::kStringified);
  auto rhs_stringified_it = rhs.metadata_.find(MetadataType::kStringified);
  bool stringified_equal = false;
  if (stringified_it == metadata_.end() ||
      rhs_stringified_it == rhs.metadata_.end() ||
      *stringified_it == *rhs_stringified_it) {
    return priority_ < rhs.priority_;
  }
  return *stringified_it < *rhs_stringified_it;
}

void Token::Print(std::ostream &out) const {
  out << "Token {";
  auto stringified_it = metadata_.find(MetadataType::kStringified);
  if (stringified_it == metadata_.end()) {
    out << "Unknown";
  } else {
    out << stringified_it->second;
  }
  out << ", " << id_ << "}";
}

std::ostream &operator<<(std::ostream &out, const Token &token) {
  token.Print(out);
  return out;
}

Token::operator bool() const { return priority_ != TokenPriority::kUnmatched; }

TokenId Token::id() const { return id_; }

TokenPriority Token::priority() const { return priority_; }

const std::map<MetadataType, std::string> &Token::metadata() const {
  return metadata_;
}

bool operator==(const Token &lhs, const Token &rhs) { return lhs.Equals(rhs); }

bool operator!=(const Token &lhs, const Token &rhs) { return !lhs.Equals(rhs); }

bool operator>(const Token &lhs, const Token &rhs) { return rhs.Less(lhs); }

bool operator>=(const Token &lhs, const Token &rhs) { return !lhs.Less(rhs); }

bool operator<(const Token &lhs, const Token &rhs) { return lhs.Less(rhs); }

bool operator<=(const Token &lhs, const Token &rhs) { return !rhs.Less(lhs); }

}  // namespace kaleidoc
