#include "token.h"

#include <string>

#include "driver.h"

namespace kaleidoc {

// Identifier token

IdentifierToken::IdentifierToken(const std::string &name) : name_{ name } {}

TokenId IdentifierToken::GetToken() const noexcept {
  return TokenId::kIdentifierId;
}

std::string IdentifierToken::GetValue() const noexcept { return name_; }

Token *IdentifierToken::Clone() const { return new IdentifierToken{ name_ }; }

// Keyword token

KeywordToken::KeywordToken(TokenId token_id, const std::string &name)
    : token_id_{ token_id }, name_{ name } {}

TokenId KeywordToken::GetToken() const noexcept { return token_id_; }

std::string KeywordToken::GetValue() const noexcept { return name_; }

Token *KeywordToken::Clone() const {
  return new KeywordToken{ token_id_, name_ };
}

// Number token

NumberToken::NumberToken(const std::string &value) : value_{ value } {}

TokenId NumberToken::GetToken() const noexcept { return TokenId::kNumberId; }

std::string NumberToken::GetValue() const noexcept { return value_; }

Token *NumberToken::Clone() const { return new NumberToken{ value_ }; }

// Binary operator token

BinaryOperatorToken::BinaryOperatorToken(const std::string &operation)
    : operator_{ operation } {}

TokenId BinaryOperatorToken::GetToken() const noexcept {
  return TokenId::kBinaryOperationId;
}

std::string BinaryOperatorToken::GetValue() const noexcept { return operator_; }

Token *BinaryOperatorToken::Clone() const {
  return new NumberToken{ operator_ };
}

// EOF token

TokenId EofToken::GetToken() const noexcept {
  return TokenId::kEofId;
}

std::string EofToken::GetValue() const noexcept { return "EOF"; }

Token *EofToken::Clone() const {
  return new EofToken;
}

}  // namespace kaleidoc
