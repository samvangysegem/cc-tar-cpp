#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <iostream>

#include "common.hpp"
#include "detail.hpp"

TEST_CASE("Tar header serialisation & deserialisation",
          "[header-serialisation]") {
  using namespace cc::tar;

  static constexpr std::uint16_t HEADER_SIZE_B = 257;

  static constexpr const char *FILE_NAME = "test";
  static constexpr std::uint64_t FILE_SIZE = 100;
  static constexpr std::uint64_t FILE_MODE = 72;
  static constexpr std::uint64_t USER_ID = 1829;
  static constexpr std::uint64_t GROUP_ID = 4;
  static constexpr std::uint8_t LINK_ID = 32;
  static constexpr const char *LINKED_FILE_NAME = "linked_test";

  common::ObjectHeader header{.fileName = FILE_NAME,
                              .fileSize = FILE_SIZE,
                              .fileMode = FILE_MODE,
                              .userID = USER_ID,
                              .groupID = GROUP_ID,
                              .linkIndicator = LINK_ID,
                              .linkedFileName = LINKED_FILE_NAME};

  SECTION("Serialise header to buffer of sufficient size") {
    std::array<char, 512> buffer{0x00};
    auto result = detail::SerialiseHeader(header, buffer);
    REQUIRE(result);
  }

  SECTION("Serialise header to buffer of insufficient size") {
    std::array<char, HEADER_SIZE_B - 10> buffer{0x00};
    auto result = detail::SerialiseHeader(header, buffer);
    REQUIRE(!result);
  }

  SECTION("Serialise header to buffer of sufficient size") {
    std::array<char, 512> buffer{0x00};
    auto result = detail::SerialiseHeader(header, buffer);
    REQUIRE(result);
  }

  SECTION("Validate successive serialisation and deserialisation of header") {
    std::array<char, 512> buffer{0x00};
    auto result = detail::SerialiseHeader(header, buffer);
    REQUIRE(result);

    auto parseResult = detail::ParseHeader(buffer);
    REQUIRE(parseResult);

    auto newHeader = parseResult.value();
    REQUIRE(newHeader.fileName.compare(FILE_NAME) == 0);
    REQUIRE(newHeader.fileSize == FILE_SIZE);
    REQUIRE(newHeader.fileMode == FILE_MODE);
    REQUIRE(newHeader.userID == USER_ID);
    REQUIRE(newHeader.groupID == GROUP_ID);
    REQUIRE(newHeader.linkIndicator == LINK_ID);
    REQUIRE(newHeader.linkedFileName.compare(LINKED_FILE_NAME) == 0);
  }

  // TODO
  // Invalid checksum
  // fileName too long
}
