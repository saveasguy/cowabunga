#ifndef KALEIDOC_SRC_PARSER_H_
#define KALEIDOC_SRC_PARSER_H_

#include <memory>

#include "driver.h"

namespace kaleidoc {

class TopLevelParser : public Parser {
 public:
  std::unique_ptr<AstNode> Parse(
      std::vector<Token>::const_iterator begin,
      std::vector<Token>::const_iterator end) const override;

  std::unique_ptr<Parser> Clone() const override;
};

}  // namespace kaleidoc

#endif  // KALEIDOC_SRC_PARSER_H_
