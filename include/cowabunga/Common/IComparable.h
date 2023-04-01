#ifndef COWABUNGA_COMMON_ICOMPARABLE_H
#define COWABUNGA_COMMON_ICOMPARABLE_H

#include <memory>

namespace cb {

template <class Derived> class IComparable {
public:
  virtual int compare(const Derived &RHS) const = 0;

  virtual ~IComparable() = default;
};

template <class Derived>
bool operator==(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return LHS.compare(static_cast<const Derived &>(RHS)) == 0;
}

template <class Derived>
bool operator!=(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return LHS.compare(static_cast<const Derived &>(RHS));
}

template <class Derived>
bool operator<(const IComparable<Derived> &LHS,
               const IComparable<Derived> &RHS) {
  return LHS.compare(static_cast<const Derived &>(RHS)) < 0;
}

template <class Derived>
bool operator<=(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return LHS.compare(static_cast<const Derived &>(RHS)) <= 0;
}

template <class Derived>
bool operator>(const IComparable<Derived> &LHS,
               const IComparable<Derived> &RHS) {
  return LHS.compare(static_cast<const Derived &>(RHS)) > 0;
}

template <class Derived>
bool operator>=(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return LHS.compare(static_cast<const Derived &>(RHS)) >= 0;
}

} // namespace cb

#endif // COWABUNGA_COMMON_ICOMPARABLE_H
