#include "cowabunga/Lexer/Token.h"

#include <cassert>
#include <limits>

using namespace cb;

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
  Out << "'" << lexeme() << "' <line: " << LineNumber << ", col: " << BeginColumnNumber
      << ":" << EndColumnNumber - 1 << ">";
}

std::string Token::lexeme() const {
  assert(Line && "Token's Line member is nullptr");
  return Line->substr(BeginColumnNumber - 1, EndColumnNumber - BeginColumnNumber);
}

int Token::id() const noexcept { return ID; }

int Token::priority() const noexcept { return Priority; }

bool Token::initialized() const noexcept { return Initialized; }

Token::operator bool() const noexcept { return Initialized; }
