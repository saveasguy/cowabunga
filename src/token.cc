#include "token.h"

#include <map>
#include <string>

#include "driver.h"

namespace kaleidoc {

Token::Token(TokenId token_id, std::string matched_string,
             TokenPriority token_priority)
    : id_{token_id},
      stringified_{std::move(matched_string)},
      priority_{token_priority} {}

void Token::AddMetadata(MetadataType type, std::string value) {
  metadata_[type] = std::move(value);
}

bool Token::Equals(const Token &rhs) const {
  return stringified_ == rhs.stringified_ && priority_ == rhs.priority_;
}

bool Token::Less(const Token &rhs) const {
  if (stringified_ == rhs.stringified_) { return priority_ < rhs.priority_; }
  return stringified_ < rhs.stringified_;
}

Token::operator bool() const { return priority_ != TokenPriority::kUnmatched; }

TokenId Token::id() const { return id_; }

TokenPriority Token::priority() const { return priority_; }

std::string Token::stringified() const { return stringified_; }

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
