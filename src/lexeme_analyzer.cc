#include "lexeme_analyzer.h"

#include <cctype>
#include <iostream>
#include <istream>
#include <memory>

#include "driver.h"
#include "token.h"

namespace kaleidoc {

// Identifier analyzer

TokenPriorityId IdentifierAnalyzer::CheckNextChar(char c) {
  bool match;
  if (flushed_) {
    flushed_ = false;
    match = std::isalpha(c) != 0 || c == '_';
  } else {
    match = std::isalnum(c) != 0 || c == '_';
  }
  if (!match) { return TokenPriorityId::kUnmatched; }
  return TokenPriorityId::kNormal;
}

std::unique_ptr<Token> IdentifierAnalyzer::GetTokenId(
    const std::string &word) const {
  return std::make_unique<IdentifierToken>(word);
}

void IdentifierAnalyzer::Flush() { flushed_ = true; }

std::unique_ptr<LexemeAnalyzer> IdentifierAnalyzer::Clone() const {
  return std::make_unique<IdentifierAnalyzer>(*this);
}

// Keyword analyzer

KeywordAnalyzer::KeywordAnalyzer(TokenId token_id, const std::string &keyword)
    : token_id_{ token_id }, keyword_{ keyword } {}

TokenPriorityId KeywordAnalyzer::CheckNextChar(char c) {
  current_input_ += c;
  if (keyword_.find(current_input_) != 0) {
    return TokenPriorityId::kUnmatched;
  }
  return TokenPriorityId::kHigh;
}

std::unique_ptr<Token> KeywordAnalyzer::GetTokenId(
    const std::string &word) const {
  return std::make_unique<KeywordToken>(token_id_, word);
}

void KeywordAnalyzer::Flush() { current_input_ = ""; }

std::unique_ptr<LexemeAnalyzer> KeywordAnalyzer::Clone() const {
  return std::make_unique<KeywordAnalyzer>(*this);
}

// Integral number analyzer

TokenPriorityId IntegralNumberAnalyzer::CheckNextChar(char c) {
  if (std::isdigit(c) != 0) { return TokenPriorityId::kUnmatched; }
  return TokenPriorityId::kNormal;
}

std::unique_ptr<Token> IntegralNumberAnalyzer::GetTokenId(
    const std::string &word) const {
  return std::make_unique<IntegralNumberToken>(word);
}

void IntegralNumberAnalyzer::Flush() {}

std::unique_ptr<LexemeAnalyzer> IntegralNumberAnalyzer::Clone() const {
  return std::make_unique<IntegralNumberAnalyzer>(*this);
}

}  // namespace kaleidoc
