#pragma once

#include <cstdint>
#include <string>

namespace cc::tar::common {

struct TarHeader {
  std::string fileName;
  std::uint64_t fileSize;
  std::uint64_t fileMode;

  std::uint64_t ownerID;
  std::uint64_t groupID;
  std::uint64_t checkSum;

  uint8_t linkIndicator;
  std::string linkedFileName;
};

} // namespace cc::tar::common
