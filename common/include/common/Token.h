#ifndef COMMON_TOKEN_H
#define COMMON_TOKEN_H

#include "common/IComparable.h"
#include "common/IPrintable.h"
#include "common/Metadata.h"
#include <map>
#include <string>

namespace cb {

namespace common {

class Token final : public common::IComparable<Token>,
                    public common::IPrintable {
public:
  Token();

  Token(int TokenID, int TokenPriority = 0);

  bool equals(const Token &RHS) const noexcept;

  bool less(const Token &RHS) const noexcept;

  void print(std::ostream &Out) const;

  common::Metadata &metadata() noexcept;

  common::Metadata metadata() const;

  std::string metadata(MetadataType Type) const;

  int id() const noexcept;

  int priority() const noexcept;

  bool initialized() const noexcept;

  operator bool() const noexcept;

private:
  common::Metadata MetadataStorage;
  int ID;
  int Priority;
  bool Initialized;
};

} // namespace common

} // namespace cb

#endif
