#ifndef COWABUNGA_PARSER_MATCHERS_H
#define COWABUNGA_PARSER_MATCHERS_H

#include "cowabunga/Lexer/Token.h"
#include "cowabunga/Parser/IParser.h"

#include <utility>
#include <vector>

namespace cb {

class IMatcher {
public:
  virtual TokenIterator match(TokenIterator ItBegin, TokenIterator ItEnd) = 0;

  virtual ~IMatcher();
};

template <int TokenID> class Terminal : public IMatcher {
public:
  TokenIterator match(TokenIterator ItBegin, TokenIterator ItEnd) override {
    if (ItBegin == ItEnd || ItBegin->id() != TokenID) {
      return ItBegin;
    }
    return ItBegin + 1;
  }
};

template <int RangeBeginID, int RangeEndID>
class TerminalInRange : public IMatcher {
public:
  TokenIterator match(TokenIterator ItBegin, TokenIterator ItEnd) override {
    if (ItBegin == ItEnd || ItBegin->id() <= RangeBeginID ||
        ItBegin->id() >= RangeEndID) {
      return ItBegin;
    }
    return ItBegin + 1;
  }
};

template <class TFirstMatcher, class... TOtherMatchers>
class Sequence : public IMatcher {
public:
  TokenIterator match(TokenIterator ItBegin, TokenIterator ItEnd) override {
    return matchSequence<TFirstMatcher, TOtherMatchers...>(ItBegin, ItEnd);
  }

  template <class TCallback>
  void callbackOnMatched(TokenIterator ItMatchedBegin,
                         TokenIterator ItMatchedEnd, TCallback &&Callback) {
    callbackOnMatchedSequence<TCallback, TFirstMatcher, TOtherMatchers...>(
        ItMatchedBegin, ItMatchedEnd, std::forward<TCallback>(Callback));
  }

private:
  template <class TLHSMatcher, class TRHSMatcher, class... TRemainingMatchers>
  TokenIterator matchSequence(TokenIterator ItBegin, TokenIterator ItEnd) {
    TLHSMatcher LHSMatcher;
    auto ItLHSMatchedEnd = LHSMatcher.match(ItBegin, ItEnd);
    if (ItLHSMatchedEnd == ItBegin) {
      return ItBegin;
    }
    auto ItOtherMatchedEnd = matchSequence<TRHSMatcher, TRemainingMatchers...>(
        ItLHSMatchedEnd, ItEnd);
    if (ItOtherMatchedEnd == ItLHSMatchedEnd) {
      return ItBegin;
    }
    return ItOtherMatchedEnd;
  }

  template <class TMatcher>
  TokenIterator matchSequence(TokenIterator ItBegin, TokenIterator ItEnd) {
    TMatcher Rule;
    return Rule.match(ItBegin, ItEnd);
  }

  template <class TCallback, class TLHSMatcher, class TRHSMatcher,
            class... TRemainingMatchers>
  void callbackOnMatchedSequence(TokenIterator ItBegin, TokenIterator ItEnd,
                                 TCallback &&Callback) {
    TLHSMatcher LHSMatcher;
    auto ItLHSMatchedEnd = LHSMatcher.match(ItBegin, ItEnd);
    Callback(LHSMatcher, ItBegin, ItLHSMatchedEnd);
    callbackOnMatchedSequence<TCallback, TRHSMatcher, TRemainingMatchers...>(
        ItLHSMatchedEnd, ItEnd, std::forward<TCallback>(Callback));
  }

  template <class TCallback, class TMatcher>
  void callbackOnMatchedSequence(TokenIterator ItBegin, TokenIterator ItEnd,
                                 TCallback &&Callback) {
    TMatcher Matcher;
    Callback(Matcher, ItBegin, ItEnd);
  }
};

template <class TFirstMatcher, class TSecondMatcher, class... TOtherMatchers>
class Optional : public IMatcher {
public:
  TokenIterator match(TokenIterator ItBegin, TokenIterator ItEnd) override {
    if (ItBegin == ItEnd) {
      return ItBegin;
    }
    return matchOptions<TFirstMatcher, TSecondMatcher, TOtherMatchers...>(
        ItBegin, ItEnd);
  }

  template <class TCallback>
  void callbackOnMatched(TokenIterator ItMatchedBegin,
                         TokenIterator ItMatchedEnd, TCallback &&Callback) {
    callbackOnExactMatch<TCallback, TFirstMatcher, TSecondMatcher,
                         TOtherMatchers...>(ItMatchedBegin, ItMatchedEnd,
                                            std::forward<TCallback>(Callback));
  }

private:
  template <class TLHSRule, class TRHSRule, class... TRemainingRules>
  TokenIterator matchOptions(TokenIterator ItBegin, TokenIterator ItEnd) {
    TLHSRule LHSRule;
    auto ItLHSMatchedEnd = LHSRule.match(ItBegin, ItEnd);
    auto ItRHSMatchedEnd =
        matchOptions<TRHSRule, TRemainingRules...>(ItBegin, ItEnd);
    if (ItLHSMatchedEnd - ItRHSMatchedEnd < 0) {
      return ItRHSMatchedEnd;
    }
    return ItLHSMatchedEnd;
  }

  template <class TRule>
  TokenIterator matchOptions(TokenIterator ItBegin, TokenIterator ItEnd) {
    TRule Rule;
    return Rule.match(ItBegin, ItEnd);
  }

  template <class TCallback, class TLHSMatcher, class TRHSMatcher,
            class... TRemainingMatchers>
  void callbackOnExactMatch(TokenIterator ItMatchedBegin,
                            TokenIterator ItMatchedEnd, TCallback &&Callback) {
    TLHSMatcher LHSMatcher;
    auto ItLHSMatchedEnd = LHSMatcher.match(ItMatchedBegin, ItMatchedEnd);
    if (ItLHSMatchedEnd == ItMatchedEnd) {
      Callback(LHSMatcher, ItMatchedBegin, ItMatchedEnd);
      return;
    }
    callbackOnExactMatch<TCallback, TRHSMatcher, TRemainingMatchers...>(
        ItMatchedBegin, ItMatchedEnd, std::forward<TCallback>(Callback));
  }

  template <class TCallback, class TMatcher>
  void callbackOnExactMatch(TokenIterator ItBegin, TokenIterator ItEnd,
                            TCallback &&Callback) {
    TMatcher Matcher;
    Callback(Matcher, ItBegin, ItEnd);
  }
};

} // namespace cb

#endif // COWABUNGA_PARSER_MATCHERS_H
