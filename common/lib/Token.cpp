#include "common/Token.h"

#include "common/Metadata.h"

#include <limits>
#include <ostream>

namespace cb {

namespace common {

Token::Token()
    : Priority(std::numeric_limits<int>::min()), Initialized(false) {}

Token::Token(int TokenID, int TokenPriority)
    : ID(TokenID), Priority(TokenPriority), Initialized(true) {}

bool Token::equals(const Token &RHS) const noexcept {
  return Initialized == RHS.Initialized && ID == RHS.ID &&
         Priority == RHS.Priority;
}

bool Token::less(const Token &RHS) const noexcept {
  return !Initialized && RHS.Initialized || Priority < RHS.Priority;
}

void Token::print(std::ostream &Out) const {
  Out << "Token {" << MetadataStorage.get(common::Stringified) << ", " << ID
      << "}";
}

int Token::id() const noexcept { return ID; }

int Token::priority() const noexcept { return Priority; }

bool Token::initialized() const noexcept { return Initialized; }

Token::operator bool() const noexcept { return initialized(); }

} // namespace common

} // namespace cb
