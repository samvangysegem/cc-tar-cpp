#pragma once

#include <cstdint>
#include <string>

namespace cc::tar::common {

struct ObjectHeader {
  std::string fileName;
  std::uint64_t fileSize;
  std::uint64_t fileMode;

  std::uint64_t userID;
  std::uint64_t groupID;
  std::uint64_t checkSum;

  uint8_t linkIndicator;
  std::string linkedFileName;
};

} // namespace cc::tar::common
