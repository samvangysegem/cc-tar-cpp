#include "detail.hpp"

#include "boost/leaf/error.hpp"
#include "common.hpp"
#include "error.hpp"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <span>

namespace cc::tar::detail {

static constexpr std::uint16_t HEADER_SIZE_B = 257;

// Checksum helpers
std::uint64_t CalculateChecksum(std::span<char> buffer) {
  auto unsignedSum = [](std::uint64_t a, char b) {
    return std::move(a) + static_cast<uint8_t>(b);
  };

  return std::accumulate(buffer.begin(), buffer.end(), 0, unsignedSum) -
         std::accumulate(buffer.begin() + common::CHECKSUM::offset,
                         buffer.begin() + common::CHECKSUM::offset +
                             common::CHECKSUM::size,
                         0, unsignedSum) +
         8 * static_cast<uint8_t>('0');
}

Status VerifyChecksum(std::span<char> buffer) {
  using namespace cc::tar::helpers;
  BOOST_LEAF_AUTO(headerCheckSum, Read<common::CHECKSUM>(buffer));
  auto checkSum = CalculateChecksum(buffer);

  if (checkSum != headerCheckSum)
    return NewError(InvalidChecksum{});
  return Success();
}

// Parsing and serialisation
Result<common::ObjectHeader> ParseHeader(std::span<char> buffer) {
  using namespace cc::tar::helpers;
  if (buffer.size_bytes() < HEADER_SIZE_B)
    return NewError(InvalidBufferSize{});

  BOOST_LEAF_CHECK(VerifyChecksum(buffer));

  common::ObjectHeader header{};
  BOOST_LEAF_ASSIGN(header.fileName, Read<common::FILE_NAME>(buffer));
  BOOST_LEAF_ASSIGN(header.fileSize, Read<common::FILE_SIZE>(buffer));
  BOOST_LEAF_ASSIGN(header.fileMode, Read<common::FILE_MODE>(buffer));
  BOOST_LEAF_ASSIGN(header.userID, Read<common::USER_ID>(buffer));
  BOOST_LEAF_ASSIGN(header.groupID, Read<common::GROUP_ID>(buffer));
  BOOST_LEAF_ASSIGN(header.linkIndicator, Read<common::LINK_INDICATOR>(buffer));
  BOOST_LEAF_ASSIGN(header.linkedFileName,
                    Read<common::LINKED_FILE_NAME>(buffer));
  return header;
}

Status SerialiseHeader(common::ObjectHeader const &header,
                       std::span<char> buffer) {
  using namespace cc::tar::helpers;
  if (buffer.size_bytes() < HEADER_SIZE_B)
    return NewError(InvalidBufferSize{});
  std::fill(buffer.begin(), buffer.end(), 0x00);

  BOOST_LEAF_CHECK(Write<common::FILE_NAME>(header.fileName, buffer));
  BOOST_LEAF_CHECK(Write<common::FILE_MODE>(header.fileMode, buffer));
  BOOST_LEAF_CHECK(Write<common::USER_ID>(header.userID, buffer));
  BOOST_LEAF_CHECK(Write<common::GROUP_ID>(header.groupID, buffer));
  BOOST_LEAF_CHECK(Write<common::FILE_SIZE>(header.fileSize, buffer));
  BOOST_LEAF_CHECK(Write<common::LINK_INDICATOR>(header.linkIndicator, buffer));
  BOOST_LEAF_CHECK(
      Write<common::LINKED_FILE_NAME>(header.linkedFileName, buffer));

  // Checksum
  auto checkSum = CalculateChecksum(buffer);
  BOOST_LEAF_CHECK(Write<common::CHECKSUM>(checkSum, buffer));
  return Success();
}

} // namespace cc::tar::detail
