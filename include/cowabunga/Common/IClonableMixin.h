#ifndef COWABUNGA_COMMON_ICLONABLEMIXIN_H
#define COWABUNGA_COMMON_ICLONABLEMIXIN_H

#include <memory>

namespace cb {

template <class Base, class Derived> class IClonableMixin : public Base {
public:
  std::unique_ptr<Base> clone() const override {
    return std::make_unique<Derived>(static_cast<const Derived &>(*this));
  }

  ~IClonableMixin() override = default;
};

} // namespace cb

#endif // COWABUNGA_COMMON_ICLONABLEMIXIN_H
