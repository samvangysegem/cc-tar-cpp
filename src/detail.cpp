#include "detail.hpp"

#include "boost/leaf/error.hpp"
#include "common.hpp"
#include "error.hpp"

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <span>

namespace cc::tar::detail {

// TODO
// Not a very type safe approach
// Better approach involves templates to define fields
// Would make formatting much easier as well
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

// Checksum helpers
std::uint64_t CalculateChecksum(std::span<char> buffer) {
  auto unsignedSum = [](std::uint64_t a, char b) {
    return std::move(a) + static_cast<uint8_t>(b);
  };

  return std::accumulate(buffer.begin(), buffer.end(), 0, unsignedSum) -
         std::accumulate(buffer.begin() + HeaderOffsets::CHECKSUM,
                         buffer.begin() + HeaderOffsets::CHECKSUM + 8, 0,
                         unsignedSum) +
         8 * static_cast<uint8_t>('0');
}

Status VerifyChecksum(std::span<char> buffer) {
  std::uint64_t headerCheckSum =
      strtoull(&buffer[HeaderOffsets::CHECKSUM], nullptr, 8);
  auto checkSum = CalculateChecksum(buffer);

  if (checkSum != headerCheckSum)
    return NewError(InvalidChecksum{});
  return Success();
}

// Parsing and serialisation
Result<common::ObjectHeader> ParseHeader(std::span<char> buffer) {
  if (buffer.size_bytes() < HEADER_SIZE_B)
    return NewError(InvalidBufferSize{});

  BOOST_LEAF_CHECK(VerifyChecksum(buffer));

  common::ObjectHeader header{};
  header.fileName = std::string(buffer.data());
  header.fileSize = strtoull(&buffer[HeaderOffsets::FILE_SIZE], nullptr, 8);
  header.fileMode = strtoull(&buffer[HeaderOffsets::FILE_MODE], nullptr, 8);
  header.userID = strtoull(&buffer[HeaderOffsets::USER_ID], nullptr, 8);
  header.groupID = strtoull(&buffer[HeaderOffsets::GROUP_ID], nullptr, 8);
  header.linkIndicator = buffer[HeaderOffsets::LINK_INDICATOR] - '0';
  header.linkedFileName =
      std::string(buffer.data() + HeaderOffsets::LINKED_FILE_NAME);
  return header;
}

inline Status ToOctalCharsN(std::uint64_t value, char *destination,
                            std::uint64_t maxLength) {
  auto [ptr, ec] =
      std::to_chars(destination, destination + maxLength, value, 8);
  if (ec != std::errc()) {
    return NewError(InvalidConversion{});
  }
  return Success();
}

Status SerialiseHeader(common::ObjectHeader const &header,
                       std::span<char> buffer) {
  if (buffer.size_bytes() < HEADER_SIZE_B)
    return NewError(InvalidBufferSize{});

  std::fill(buffer.begin(), buffer.end(), 0x00);

  // Filename && Linked filename
  std::copy_n(header.fileName.c_str(), header.fileName.size() + 1,
              buffer.begin() + HeaderOffsets::FILE_NAME);
  std::copy_n(header.linkedFileName.c_str(), header.linkedFileName.size() + 1,
              buffer.begin() + HeaderOffsets::LINKED_FILE_NAME);

  // Octal conversions
  BOOST_LEAF_CHECK(ToOctalCharsN(header.fileSize,
                                 buffer.data() + HeaderOffsets::FILE_SIZE, 12));
  BOOST_LEAF_CHECK(ToOctalCharsN(header.fileSize,
                                 buffer.data() + HeaderOffsets::FILE_SIZE, 12));
  BOOST_LEAF_CHECK(ToOctalCharsN(header.fileMode,
                                 buffer.data() + HeaderOffsets::FILE_MODE, 8));
  BOOST_LEAF_CHECK(
      ToOctalCharsN(header.userID, buffer.data() + HeaderOffsets::USER_ID, 8));
  BOOST_LEAF_CHECK(ToOctalCharsN(header.groupID,
                                 buffer.data() + HeaderOffsets::GROUP_ID, 8));

  // Link indicator
  buffer[HeaderOffsets::LINK_INDICATOR] = header.linkIndicator + '0';

  // Checksum
  auto checkSum = CalculateChecksum(buffer);
  BOOST_LEAF_CHECK(
      ToOctalCharsN(checkSum, buffer.data() + HeaderOffsets::CHECKSUM, 8));
  return Success();
}

} // namespace cc::tar::detail
