#include "driver.h"

#include <algorithm>

namespace kaleidoc {

TokenPriorityId LexemeAnalyzer::CheckWholeWord(const std::string &word) {
  TokenPriorityId priority = TokenPriorityId::kUnmatched;
  for (auto c : word) { priority = CheckNextChar(c); }
  return priority;
}

}  // namespace kaleidoc
