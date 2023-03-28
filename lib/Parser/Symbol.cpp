#include "cowabunga/Parser/Symbol.h"

using namespace cb;

Symbol::Symbol() : ID(0), IsTerminal(false){};

Symbol::Symbol(int SymbolID, bool Terminal)
    : ID(SymbolID), IsTerminal(Terminal) {}

bool Symbol::isTerminal() const noexcept { return IsTerminal; }

bool Symbol::isNonTerminal() const noexcept { return !IsTerminal; }

int Symbol::getID() const noexcept { return ID; }

int Symbol::compare(const Symbol &RHS) const {
    return ID - RHS.ID;
}
