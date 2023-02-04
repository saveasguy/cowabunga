#include "lexeme_analyzer.h"

#include <cctype>
#include <iostream>
#include <istream>

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

Token *IdentifierAnalyzer::GetToken(const std::string &word) const {
  return new IdentifierToken{ word };
}

void IdentifierAnalyzer::Flush() { flushed_ = true; }

LexemeAnalyzer *IdentifierAnalyzer::Clone() const {
  return new IdentifierAnalyzer;
}

// Keyword analyzer

KeywordAnalyzer::KeywordAnalyzer(TokenId token_id, const std::string &keyword)
    : token_id_{ token_id }, keyword_{ keyword } {}

TokenPriority KeywordAnalyzer::CheckNextChar(char c) {
  current_input_ += c;
  if (keyword_.find(current_input_) != 0) { return TokenPriority::kUnmatched; }
  return TokenPriority::kKeyword;
}

Token *KeywordAnalyzer::GetToken(const std::string &word) const {
  return new KeywordToken{ token_id_, word };
}

void KeywordAnalyzer::Flush() { current_input_ = ""; }

LexemeAnalyzer *KeywordAnalyzer::Clone() const {
  return new KeywordAnalyzer{ token_id_, keyword_ };
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

Token *NumberAnalyzer::GetToken(const std::string &word) const {
  return new NumberToken{ word };
}

void NumberAnalyzer::Flush() { point_used_ = false; }

LexemeAnalyzer *NumberAnalyzer::Clone() const { return new NumberAnalyzer; }

// Binary operator analyzer

BinaryOperatorAnalyzer::BinaryOperatorAnalyzer(const std::string &operation)
    : operator_{ operation } {}

TokenPriority BinaryOperatorAnalyzer::CheckNextChar(char c) {
  current_input_ += c;
  bool match = operator_.find(current_input_) == 0;
  if (!match) { return TokenPriority::kUnmatched; }
  return TokenPriority::kNormal;
}

Token *BinaryOperatorAnalyzer::GetToken(const std::string &word) const {
  return new BinaryOperatorToken{ word };
}

void BinaryOperatorAnalyzer::Flush() { current_input_ = ""; }

LexemeAnalyzer *BinaryOperatorAnalyzer::Clone() const {
  return new BinaryOperatorAnalyzer{ operator_ };
}

}  // namespace kaleidoc
