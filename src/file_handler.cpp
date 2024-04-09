#include "file_handler.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>

#include "boost/leaf/error.hpp"
#include "common.hpp"
#include "detail.hpp"
#include "error.hpp"

namespace cc::tar {

bool FileHandler::IsValid() noexcept {
  auto validExtension =
      mTarFilePath.find(".tar", mTarFilePath.size() - 4) != std::string::npos;
  return validExtension;
}

Result<std::vector<common::ObjectHeader>> FileHandler::ListContents() noexcept {
  if (!IsValid()) {
    return NewError(InvalidFile{});
  }

  // TODO: Introduce scopeguard class for file management
  std::ifstream tarFile(mTarFilePath, std::ios::binary);
  if (!tarFile) {
    return NewError(InvalidStream{});
  }

  std::vector<common::ObjectHeader> output{};
  std::vector<char> buffer(CHUNK_SIZE_B);
  while (tarFile.read(buffer.data(), CHUNK_SIZE_B) || tarFile.gcount()) {
    BOOST_LEAF_AUTO(header, detail::ParseHeader(buffer));
    output.push_back(header);

    auto fileSizeChunks = header.fileSize / CHUNK_SIZE_B + 1;
    tarFile.seekg(fileSizeChunks * CHUNK_SIZE_B, std::ios_base::cur);
  }

  tarFile.close();

  return {output};
}

Status FileHandler::Extract() noexcept {
  if (!IsValid()) {
    return NewError(InvalidFile{});
  }

  std::ifstream tarFile(mTarFilePath, std::ios::binary);
  if (!tarFile) {
    return NewError(InvalidStream{});
  }

  std::vector<char> buffer(CHUNK_SIZE_B);
  while (tarFile.read(buffer.data(), CHUNK_SIZE_B) || tarFile.gcount()) {
    BOOST_LEAF_AUTO(header, detail::ParseHeader(buffer));

    // Validate file path
    if (header.fileName.find("../", 0) != std::string::npos) {
      return NewError(InvalidContents{.fileName = header.fileName,
                                      .description = "Path contains \"..\""});
    }

    // Create new file with object contents
    std::ofstream extractedFile(header.fileName, std::ios::binary);
    if (!extractedFile)
      return NewError(InvalidStream{});

    for (std::uint64_t offset = 0; offset < header.fileSize;
         offset += CHUNK_SIZE_B) {
      // Read full chunk from tar file
      tarFile.read(buffer.data(), CHUNK_SIZE_B);

      // Write useful contents to extracted file
      auto remainingSize = std::min(header.fileSize - offset, CHUNK_SIZE_B);
      extractedFile.write(buffer.data(), remainingSize);
    }

    extractedFile.close();
  }

  tarFile.close();
  return Success();
}

Status FileHandler::Compress(std::vector<std::string> filePaths) noexcept {
  if (!IsValid()) {
    return NewError(InvalidFile{});
  }

  std::ofstream tarFile(mTarFilePath, std::ios::binary);
  if (!tarFile) {
    return NewError(InvalidStream{});
  }

  std::array<char, CHUNK_SIZE_B> buffer{0x00};
  for (auto const &filePath : filePaths) {
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile)
      return NewError(InvalidFile{});

    // Extract header
    // Write method for extracting header info from filename
    common::ObjectHeader header{};
    header.fileName = filePath;
    header.fileSize = 120;

    // Write to output buffer
    BOOST_LEAF_CHECK(detail::SerialiseHeader(header, buffer));
    tarFile.write(buffer.data(), buffer.size());

    std::fill(buffer.begin(), buffer.end(), 0x00);
    while (inputFile.read(buffer.data(), CHUNK_SIZE_B) || inputFile.gcount()) {
      tarFile.write(buffer.data(), CHUNK_SIZE_B);
      std::fill(buffer.begin(), buffer.end(), 0x00);
    }

    inputFile.close();
  }

  tarFile.close();

  return Success();
}

} // namespace cc::tar
