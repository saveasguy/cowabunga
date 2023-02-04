#include "driver.h"

#include <algorithm>

namespace kaleidoc {

TokenPriority LexemeAnalyzer::CheckWholeWord(const std::string &word) {
  TokenPriority priority = TokenPriority::kUnmatched;
  for (auto c: word) {
    priority = CheckNextChar(c);
  }
  return priority;
}

}  // namespace kaleidoc
