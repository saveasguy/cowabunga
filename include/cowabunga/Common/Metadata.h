#ifndef COWABUNGA_COMMON_METADATA_H
#define COWABUNGA_COMMON_METADATA_H

#include <map>
#include <string>

namespace cb {

enum MetadataType { Stringified };

class Metadata final {
public:
  void set(MetadataType Type, std::string Value);

  std::string get(MetadataType Type) const;

private:
  std::map<MetadataType, std::string> DataStorage;
};

} // namespace cb

#endif // COWABUNGA_COMMON_METADATA_H
