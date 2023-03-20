#ifndef COWABUNGA_LEXER_TOKEN_H
#define COWABUNGA_LEXER_TOKEN_H

#include "cowabunga/Common/IComparable.h"
#include "cowabunga/Common/IPrintable.h"
#include "cowabunga/Common/Metadata.h"

#include <map>
#include <string>

namespace cb {

class Token final : public IComparable<Token>, public IPrintable {
public:
  Token();

  Token(int LexemeID, int LexemePriority = 0);

  bool equals(const Token &RHS) const noexcept;

  bool less(const Token &RHS) const noexcept;

  void print(std::ostream &Out) const;

  int id() const noexcept;

  int priority() const noexcept;

  bool initialized() const noexcept;

  operator bool() const noexcept;

  Metadata MetadataStorage;

private:
  int ID;
  int Priority;
  bool Initialized;
};

} // namespace cb

#endif // COWABUNGA_LEXER_TOKEN_H
