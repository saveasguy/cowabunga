#ifndef COWABUNGA_LEXER_TOKEN_H
#define COWABUNGA_LEXER_TOKEN_H

#include "cowabunga/Common/IComparable.h"
#include "cowabunga/Common/IPrintable.h"

namespace cb {

class Token final : public IComparable<Token>, public IPrintable {
public:
  Token();

  Token(int LexemeID, int LexemePriority = 0);

  bool equals(const Token &RHS) const noexcept;

  bool less(const Token &RHS) const noexcept;

  void print(std::ostream &Out) const;

  std::string lexeme() const;

  int id() const noexcept;

  int priority() const noexcept;

  bool initialized() const noexcept;

  operator bool() const noexcept;

  std::shared_ptr<const std::string> File;
  std::shared_ptr<const std::string> Line;
  size_t LineNumber;
  size_t BeginColumnNumber;
  size_t EndColumnNumber;

private:
  int ID;
  int Priority;
  bool Initialized;
};

} // namespace cb

#endif // COWABUNGA_LEXER_TOKEN_H
