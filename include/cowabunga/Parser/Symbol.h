#ifndef COWABUNGA_PARSER_SYMBOL_H
#define COWABUNGA_PARSER_SYMBOL_H

#include "cowabunga/Common/IComparable.h"

namespace cb {

class Symbol final : public IComparable<Symbol> {
public:
  Symbol();

  explicit Symbol(int SymbolID, bool Terminal = true);

  bool isTerminal() const noexcept;

  bool isNonTerminal() const noexcept;

  int getID() const noexcept;

  int compare(const Symbol &RHS) const override;

private:
  int ID;
  bool IsTerminal;
};

inline Symbol terminal(int SymbolID) {
    return Symbol(SymbolID);
}

inline Symbol nonTerminal(int SymbolID) {
    return Symbol(SymbolID, false);
}

} // namespace cb

#endif // COWABUNGA_PARSER_SYMBOL_H
