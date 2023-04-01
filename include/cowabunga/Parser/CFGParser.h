#ifndef COWABUNGA_PARSER_CFG_PARSER_H
#define COWABUNGA_PARSER_CFG_PARSER_H

#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/ICFGRule.h"
#include "cowabunga/Parser/Symbol.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <stack>
#include <vector>

namespace cb {

class CFGParser final {
public:
  CFGParser(Symbol StartNonTerminal);

  CFGParser(const CFGParser &RHS);

  CFGParser(CFGParser &&RHS) noexcept = default;

  CFGParser &operator=(const CFGParser &RHS);

  CFGParser &operator=(CFGParser &&RHS) noexcept = default;

  template <class TRule>
  CFGParser &addCFGRule(TRule &&RuleImpl) {
    std::unique_ptr<ICFGRule> NewRule = std::make_unique<TRule>(std::forward<TRule>(RuleImpl));
    auto ItInsertTo = std::upper_bound(Rules.begin(), Rules.end(), NewRule,
                                     [](const std::unique_ptr<ICFGRule> &LHS,
                                        const std::unique_ptr<ICFGRule> &RHS) {
                                       return LHS->getLHSNonTerminal() <
                                              RHS->getLHSNonTerminal();
                                     });
  Rules.insert(ItInsertTo, std::move(NewRule));
  return *this;
  }

  void parse(TokenIterator ItBegin, TokenIterator ItEnd);

private:
  std::vector<std::unique_ptr<ICFGRule>> Rules;
  Symbol StartSymbol;
};

} // namespace cb

#endif // COWABUNGA_PARSER_CFG_PARSER_H
