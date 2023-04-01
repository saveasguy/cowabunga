#ifndef COWABUNGA_LEXER_TOKEN_H
#define COWABUNGA_LEXER_TOKEN_H

#include "cowabunga/Common/IComparable.h"
#include "cowabunga/Common/IPrintable.h"

namespace cb {

/// Token class represents output of Lexer. Token's ID defines identity and
/// ordering of tokens.
class Token final : public IComparable<Token>, public IPrintable {
public:
  Token(int LexemeID);

  int compare(const Token &RHS) const;

  void print(std::ostream &Out) const;

  std::string getFile() const;

  std::string getLine() const;

  std::string getLexeme() const;

  int getID() const noexcept;

  std::shared_ptr<const std::string> File;
  std::shared_ptr<const std::string> Line;
  size_t LineNumber;
  size_t BeginColumnNumber;
  size_t EndColumnNumber;

private:
  int ID;
};

} // namespace cb

#endif // COWABUNGA_LEXER_TOKEN_H
