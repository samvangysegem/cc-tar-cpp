#include "detail.hpp"

#include <cstdlib>
#include <span>
#include <string>

namespace cc::tar::detail {

struct HeaderOffsets {
  static constexpr std::uint16_t FILE_NAME = 0;
  static constexpr std::uint16_t FILE_MODE = 100;
  static constexpr std::uint16_t USER_ID = 108;
  static constexpr std::uint16_t GROUP_ID = 116;
  static constexpr std::uint16_t FILE_SIZE = 124;
  static constexpr std::uint16_t LAST_MODIFIED = 136;
  static constexpr std::uint16_t CHECKSUM = 148;
  static constexpr std::uint16_t LINK_INDICATOR = 156;
  static constexpr std::uint16_t LINKED_FILE_NAME = 157;
};

static constexpr std::uint16_t HEADER_SIZE_B = 257;

common::TarHeader ParseHeader(std::span<char> buffer) {
  if (buffer.size_bytes() < HEADER_SIZE_B)
    return {};

  common::TarHeader header{};
  header.fileName = std::string(buffer.begin(), buffer.begin() + 100);
  header.fileSize = strtoull(&buffer[HeaderOffsets::FILE_SIZE], nullptr, 8);
  header.fileMode = strtoull(&buffer[HeaderOffsets::FILE_MODE], nullptr, 8);
  header.userID = strtoull(&buffer[HeaderOffsets::USER_ID], nullptr, 8);
  header.groupID = strtoull(&buffer[HeaderOffsets::GROUP_ID], nullptr, 8);
  header.checkSum = strtoull(&buffer[HeaderOffsets::CHECKSUM], nullptr, 108);
  header.linkIndicator = buffer[HeaderOffsets::LINK_INDICATOR] - '0';
  header.linkedFileName = std::string(
      buffer.begin() + HeaderOffsets::LINKED_FILE_NAME, buffer.end());
  return header;
}

} // namespace cc::tar::detail
