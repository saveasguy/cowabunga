#include "token.h"

#include <memory>
#include <string>

#include "driver.h"

namespace kaleidoc {

// Identifier token

IdentifierToken::IdentifierToken(const std::string &name) : name_{ name } {}

TokenId IdentifierToken::GetTokenId() const noexcept {
  return TokenId::kIdentifier;
}

std::string IdentifierToken::GetValue() const noexcept { return name_; }

std::unique_ptr<Token> IdentifierToken::Clone() const {
  return std::make_unique<IdentifierToken>(*this);
}

// Keyword token

KeywordToken::KeywordToken(TokenId token_id, const std::string &name)
    : token_id_{ token_id }, name_{ name } {}

TokenId KeywordToken::GetTokenId() const noexcept { return token_id_; }

std::string KeywordToken::GetValue() const noexcept { return name_; }

std::unique_ptr<Token> KeywordToken::Clone() const {
  return std::make_unique<KeywordToken>(*this);
}

// Integral number token

IntegralNumberToken::IntegralNumberToken(const std::string &value)
    : value_{ value } {}

TokenId IntegralNumberToken::GetTokenId() const noexcept {
  return TokenId::kIntegralNumber;
}

std::string IntegralNumberToken::GetValue() const noexcept { return value_; }

std::unique_ptr<Token> IntegralNumberToken::Clone() const {
  return std::make_unique<IntegralNumberToken>(*this);
}

// EOF token

TokenId EofToken::GetTokenId() const noexcept { return TokenId::kEof; }

std::string EofToken::GetValue() const noexcept { return "EOF"; }

std::unique_ptr<Token> EofToken::Clone() const {
  return std::make_unique<EofToken>(*this);
}

}  // namespace kaleidoc
