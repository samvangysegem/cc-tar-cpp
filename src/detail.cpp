#include "detail.hpp"

#include "common.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <span>

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

common::ObjectHeader ParseHeader(std::span<char> buffer) {
  if (buffer.size_bytes() < HEADER_SIZE_B)
    return {};

  common::ObjectHeader header{};
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

bool ToOctalCharsN(std::uint64_t value, char *destination,
                   std::uint64_t maxLength) {
  auto [ptr, ec] =
      std::to_chars(destination, destination + maxLength, value, 8);
  return (ec == std::errc());
}

bool SerialiseHeader(common::ObjectHeader const &header,
                     std::span<char> buffer) {
  if (buffer.size_bytes() < HEADER_SIZE_B)
    return false;

  std::fill(buffer.begin(), buffer.end(), 0x00);

  std::copy_n(header.fileName.begin(), header.fileName.size(),
              buffer.begin() + HeaderOffsets::FILE_NAME);

  bool status{true};
  status |= ToOctalCharsN(header.fileSize,
                          buffer.data() + HeaderOffsets::FILE_SIZE, 12);
  return status;
}

} // namespace cc::tar::detail
