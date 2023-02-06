#include "lexeme_analyzer.h"

#include <cctype>
#include <iostream>
#include <istream>
#include <memory>

#include "driver.h"
#include "token.h"

namespace kaleidoc {

// Identifier analyzer

TokenPriority IdentifierAnalyzer::CheckNextChar(char c) {
  bool match;
  if (flushed_) {
    flushed_ = false;
    match = std::isalpha(c) != 0 || c == '_';
  } else {
    match = std::isalnum(c) != 0 || c == '_';
  }
  if (!match) { return TokenPriority::kUnmatched; }
  return TokenPriority::kNormal;
}

std::unique_ptr<Token> IdentifierAnalyzer::GetToken(const std::string &word) const {
  return std::make_unique<IdentifierToken>(word);
}

void IdentifierAnalyzer::Flush() { flushed_ = true; }

std::unique_ptr<LexemeAnalyzer> IdentifierAnalyzer::Clone() const {
  return std::make_unique<IdentifierAnalyzer>(*this);
}

// Keyword analyzer

KeywordAnalyzer::KeywordAnalyzer(TokenId token_id, const std::string &keyword)
    : token_id_{ token_id }, keyword_{ keyword } {}

TokenPriority KeywordAnalyzer::CheckNextChar(char c) {
  current_input_ += c;
  if (keyword_.find(current_input_) != 0) { return TokenPriority::kUnmatched; }
  return TokenPriority::kHigh;
}

std::unique_ptr<Token> KeywordAnalyzer::GetToken(const std::string &word) const {
  return std::make_unique<KeywordToken>(token_id_, word);
}

void KeywordAnalyzer::Flush() { current_input_ = ""; }

std::unique_ptr<LexemeAnalyzer> KeywordAnalyzer::Clone() const {
  return std::make_unique<KeywordAnalyzer>(*this);
}

// Number analyzer

TokenPriority NumberAnalyzer::CheckNextChar(char c) {
  bool match;
  if (c == '.') {
    bool point_used_earlier = point_used_;
    point_used_ = true;
    match = !point_used_earlier;
  } else {
    match = std::isdigit(c) != 0;
  }
  if (!match) { return TokenPriority::kUnmatched; }
  return TokenPriority::kNormal;
}

std::unique_ptr<Token> NumberAnalyzer::GetToken(const std::string &word) const {
  return std::make_unique<NumberToken>(word);
}

void NumberAnalyzer::Flush() { point_used_ = false; }

std::unique_ptr<LexemeAnalyzer> NumberAnalyzer::Clone() const { return  std::make_unique<NumberAnalyzer>(*this); }

}  // namespace kaleidoc
