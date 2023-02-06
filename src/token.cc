#include "token.h"

#include <memory>
#include <string>

#include "driver.h"

namespace kaleidoc {

// Identifier token

IdentifierToken::IdentifierToken(const std::string &name) : name_{ name } {}

TokenId IdentifierToken::GetToken() const noexcept {
  return TokenId::kIdentifierId;
}

std::string IdentifierToken::GetValue() const noexcept { return name_; }

std::unique_ptr<Token> IdentifierToken::Clone() const {
  return std::make_unique<IdentifierToken>(*this);
}

// Keyword token

KeywordToken::KeywordToken(TokenId token_id, const std::string &name)
    : token_id_{ token_id }, name_{ name } {}

TokenId KeywordToken::GetToken() const noexcept { return token_id_; }

std::string KeywordToken::GetValue() const noexcept { return name_; }

std::unique_ptr<Token> KeywordToken::Clone() const {
  return std::make_unique<KeywordToken>(*this);
}

// Number token

NumberToken::NumberToken(const std::string &value) : value_{ value } {}

TokenId NumberToken::GetToken() const noexcept { return TokenId::kNumberId; }

std::string NumberToken::GetValue() const noexcept { return value_; }

std::unique_ptr<Token> NumberToken::Clone() const {
  return std::make_unique<NumberToken>(*this);
}

// EOF token

TokenId EofToken::GetToken() const noexcept { return TokenId::kEofId; }

std::string EofToken::GetValue() const noexcept { return "EOF"; }

std::unique_ptr<Token> EofToken::Clone() const {
  return std::make_unique<EofToken>(*this);
}

}  // namespace kaleidoc
