#ifndef COWABUNGA_PARSER_CFGPARSERERROR_H
#define COWABUNGA_PARSER_CFGPARSERERROR_H

#include "cowabunga/Lexer/Lexer.h"
#include "cowabunga/Parser/Symbol.h"

namespace cb {

class ICFGRule;

struct CFGParserError final {
  ICFGRule *FailedRule;
  TokenIterator ItFoundToken;
  Symbol ExpectedSymbol;
  bool EOFFound;
  bool EOFExpected;
};

} // namespace cb

#endif // COWABUNGA_PARSER_CFGPARSERERROR_H
