#include "token.h"

#include <map>
#include <memory>
#include <string>

#include "driver.h"

namespace kaleidoc {

// Identifier token

IdentifierToken::IdentifierToken(const std::string &name) {
  metadata_[MetadataType::kValue] = name;
}

TokenId IdentifierToken::GetTokenId() const noexcept {
  return TokenId::kIdentifier;
}

std::map<MetadataType, std::string> IdentifierToken::GetMetadata()
    const noexcept {
  return metadata_;
}

std::unique_ptr<Token> IdentifierToken::Clone() const {
  return std::make_unique<IdentifierToken>(*this);
}

// Keyword token

KeywordToken::KeywordToken(TokenId token_id, const std::string &name)
    : token_id_{ token_id } {
  metadata_[MetadataType::kValue] = name;
}

TokenId KeywordToken::GetTokenId() const noexcept { return token_id_; }

std::map<MetadataType, std::string> KeywordToken::GetMetadata() const noexcept {
  return metadata_;
}

std::unique_ptr<Token> KeywordToken::Clone() const {
  return std::make_unique<KeywordToken>(*this);
}

// Integral number token

IntegralNumberToken::IntegralNumberToken(const std::string &value) {
  metadata_[MetadataType::kValue] = value;
}

TokenId IntegralNumberToken::GetTokenId() const noexcept {
  return TokenId::kIntegralNumber;
}

std::map<MetadataType, std::string> IntegralNumberToken::GetMetadata()
    const noexcept {
  return metadata_;
}

std::unique_ptr<Token> IntegralNumberToken::Clone() const {
  return std::make_unique<IntegralNumberToken>(*this);
}

// EOF token

TokenId EofToken::GetTokenId() const noexcept { return TokenId::kEof; }

std::map<MetadataType, std::string> EofToken::GetMetadata() const noexcept {
  return std::map<MetadataType, std::string>{};
}

std::unique_ptr<Token> EofToken::Clone() const {
  return std::make_unique<EofToken>(*this);
}

}  // namespace kaleidoc
