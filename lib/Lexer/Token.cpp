#include "cowabunga/Lexer/Token.h"

#include <cassert>
#include <limits>

using namespace cb;

Token::Token(int TokenID) : ID(TokenID) {}

int Token::compare(const Token &RHS) const { return ID - RHS.ID; }

void Token::print(std::ostream &Out) const {
  Out << "'" << getLexeme() << "' <line: " << LineNumber
      << ", col: " << BeginColumnNumber << ":" << EndColumnNumber - 1 << ">";
}

std::string Token::getFile() const {
  assert(File && "Token's File member is nullptr");
  return *File;
}

std::string Token::getLine() const {
  assert(Line && "Token's Line member is nullptr");
  return *Line;
}

std::string Token::getLexeme() const {
  assert(Line && "Token's Line member is nullptr");
  return Line->substr(BeginColumnNumber - 1,
                      EndColumnNumber - BeginColumnNumber);
}

int Token::getID() const noexcept { return ID; }
