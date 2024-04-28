#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <string>

#include "common.hpp"
#include "detail.hpp"
#include "svgys/program_options.hpp"

TEST_CASE("Program option parser", "[option-parser]") {
  const int argc = 7;
  const char *argv[argc] = {"cc-tar",    "--compress", "test.tar", "file1.txt",
                            "file2.txt", "--seed",     "10"};

  SECTION("Valid flags") {
    using namespace svgys::program_options;

    OptionsParser parser{};
    parser.AddOptions()("compress", "compress files to tar ball")(
        "extract", "extract files from tar ball")(
        "seed", "random number used for testing");
    auto result = parser.Parse(argc, argv);
    REQUIRE(result);
  }

  SECTION("Invalid flag") {
    using namespace svgys::program_options;

    OptionsParser parser{};
    parser.AddOptions()("compress", "compress files to tar ball")(
        "files", "files to add"); // --seed flag left out
    auto result = parser.Parse(argc, argv);
    REQUIRE(!result);
  }

  SECTION("Flag parsing") {
    using namespace svgys::program_options;

    OptionsParser parser{};
    parser.AddOptions()("compress", "compress files to tar ball")(
        "extract", "extract files from tar ball")(
        "seed", "random number used for testing");
    auto result = parser.Parse(argc, argv);
    REQUIRE(result);

    REQUIRE(result.value().Contains("compress"));
    REQUIRE(result.value().Contains("seed"));
    REQUIRE(!result.value().Contains("extract"));
    REQUIRE(!result.value().Contains("cc-tar"));
  }

  SECTION("Argument parsing") {
    using namespace svgys::program_options;

    OptionsParser parser{};
    parser.AddOptions()("compress", "compress files to tar ball")(
        "extract", "extract files from tar ball")(
        "seed", "random number used for testing");
    auto result = parser.Parse(argc, argv);
    REQUIRE(result);

    auto file = result.value().AtAs<std::string>("compress");
    REQUIRE(!file);

    auto files = result.value().AtAs<std::vector<std::string>>("compress");
    REQUIRE(files);
    auto seed = result.value().AtAs<int>("seed");
    REQUIRE(seed);

    REQUIRE(files.value()[0].compare("test.tar") == 0);
    REQUIRE(files.value()[1].compare("file1.txt") == 0);
    REQUIRE(files.value()[2].compare("file2.txt") == 0);
    REQUIRE(seed.value() == 10);
  }
}

TEST_CASE("Field (de)serialisation", "[field-serialisation]") {
  using namespace cc::tar;
  std::array<char, 512> buffer{0x00};

  SECTION("String_t read & write") {
    std::string fileName{"test_file.txt"};

    auto writeResult = helpers::Write<common::FILE_NAME>(fileName, buffer);
    REQUIRE(writeResult);

    auto readResult = helpers::Read<common::FILE_NAME>(buffer);
    REQUIRE(readResult);

    REQUIRE(readResult.value().compare(fileName) == 0);
  }

  SECTION("Octal_t read & write") {
    std::uint64_t fileSize = 4096;

    auto writeResult = helpers::Write<common::FILE_SIZE>(fileSize, buffer);
    REQUIRE(writeResult);

    auto readResult = helpers::Read<common::FILE_SIZE>(buffer);
    REQUIRE(readResult);

    REQUIRE(readResult.value() == fileSize);
  }

  SECTION("EnumClass_t read & write") {
    common::LinkIndicator linkId = common::LinkIndicator::HARD_LINK;

    auto writeResult = helpers::Write<common::LINK_INDICATOR>(linkId, buffer);
    REQUIRE(writeResult);

    auto readResult = helpers::Read<common::LINK_INDICATOR>(buffer);
    REQUIRE(readResult);

    REQUIRE(readResult.value() == linkId);
  }
}

TEST_CASE("Tar header (de)serialisation", "[header-serialisation]") {
  using namespace cc::tar;

  static constexpr std::uint16_t HEADER_SIZE_B = 257;

  static constexpr const char *FILE_NAME = "test";
  static constexpr std::uint64_t FILE_SIZE = 12;
  static constexpr std::uint64_t FILE_MODE = 72;
  static constexpr std::uint64_t USER_ID = 1829;
  static constexpr std::uint64_t GROUP_ID = 4;
  static constexpr common::LinkIndicator LINK_ID =
      common::LinkIndicator::HARD_LINK;
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

  SECTION("Successive serialisation and deserialisation of header") {
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

  SECTION("Invalid checksum upon deserialisation of header") {
    std::array<char, 512> buffer{0x00};
    auto result = detail::SerialiseHeader(header, buffer);
    REQUIRE(result);

    // Change checksum value
    static constexpr std::uint32_t CHECKSUM_OFFSET = 148;
    buffer[CHECKSUM_OFFSET] += 1;

    auto parseResult = detail::ParseHeader(buffer);
    REQUIRE(!parseResult);
  }
}
