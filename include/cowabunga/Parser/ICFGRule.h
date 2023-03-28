#ifndef COWABUNGA_PARSER_ICFGRULE_H
#define COWABUNGA_PARSER_ICFGRULE_H

#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Lexer/Token.h"
#include "cowabunga/Parser/CFGParserError.h"
#include "cowabunga/Parser/Symbol.h"

#include <memory>
#include <vector>

namespace cb {

class ICFGRule {
public:
  virtual void parse(TokenIterator ItToken) = 0;

  virtual void produceError(CFGParserError Error) = 0;

  virtual Symbol getLHSNonTerminal() const = 0;

  virtual std::vector<Symbol> getProducts() const = 0;

  virtual std::unique_ptr<ICFGRule> clone() const = 0;

  virtual ~ICFGRule();
};

} // namespace cb

#endif // COWABUNGA_PARSER_ICFGRULE_H
