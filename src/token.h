#ifndef KALEIDOC_SRC_TOKEN_H_
#define KALEIDOC_SRC_TOKEN_H_

#include <map>
#include <string>

#include "driver.h"

namespace kaleidoc {

class Token final {
 public:
  Token() = default;

  Token(TokenId token_id, std::string matched_string,
        TokenPriority token_priority = TokenPriority::kNormal);

  void AddMetadata(MetadataType type, std::string value);

  bool Equals(const Token &rhs) const;

  bool Less(const Token &rhs) const;

  operator bool() const;

  TokenId id() const;

  TokenPriority priority() const;

  std::string stringified() const;

  const std::map<MetadataType, std::string> &metadata() const;

 private:
  std::map<MetadataType, std::string> metadata_;
  std::string stringified_;
  TokenId id_;
  TokenPriority priority_;
};

bool operator==(const Token &lhs, const Token &rhs);

bool operator!=(const Token &lhs, const Token &rhs);

bool operator>(const Token &lhs, const Token &rhs);

bool operator>=(const Token &lhs, const Token &rhs);

bool operator<(const Token &lhs, const Token &rhs);

bool operator<=(const Token &lhs, const Token &rhs);

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_TOKEN_H_
