#ifndef COWABUNGA_COMMON_ICOMPARABLE_H
#define COWABUNGA_COMMON_ICOMPARABLE_H

#include <memory>

namespace cb {

template <class Derived> class IComparable {
public:
  virtual bool less(const Derived &RHS) const = 0;

  virtual bool equals(const Derived &RHS) const = 0;

  virtual ~IComparable() = default;
};

template <class Derived>
bool operator==(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return LHS.equals(static_cast<const Derived &>(RHS));
}

template <class Derived>
bool operator!=(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return !LHS.equals(static_cast<const Derived &>(RHS));
}

template <class Derived>
bool operator<(const IComparable<Derived> &LHS,
               const IComparable<Derived> &RHS) {
  return LHS.less(static_cast<const Derived &>(RHS));
}

template <class Derived>
bool operator<=(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return !RHS.less(static_cast<const Derived &>(LHS));
}

template <class Derived>
bool operator>(const IComparable<Derived> &LHS,
               const IComparable<Derived> &RHS) {
  return RHS.less(static_cast<const Derived &>(LHS));
}

template <class Derived>
bool operator>=(const IComparable<Derived> &LHS,
                const IComparable<Derived> &RHS) {
  return !LHS.less(static_cast<const Derived &>(RHS));
}

} // namespace cb

#endif // COWABUNGA_COMMON_ICOMPARABLE_H
