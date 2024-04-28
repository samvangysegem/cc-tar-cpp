#include "file_handler.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include "common.hpp"
#include "detail.hpp"
#include "error_code.hpp"

namespace cc::tar {

bool FileHandler::IsValid() noexcept {
  auto validExtension =
      mTarFilePath.find(".tar", mTarFilePath.size() - 4) != std::string::npos;
  return validExtension;
}

Result<std::vector<common::ObjectHeader>> FileHandler::ListContents() noexcept {
  if (!IsValid()) {
    return NewError(error::InvalidFile{mTarFilePath});
  }

  // TODO: Introduce scopeguard class for file management
  std::ifstream tarFile(mTarFilePath, std::ios::binary);
  if (!tarFile) {
    return NewError(
        error::InvalidStream{mTarFilePath, error::StreamType::INPUT});
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
    return NewError(error::InvalidFile{mTarFilePath});
  }

  std::ifstream tarFile(mTarFilePath, std::ios::binary);
  if (!tarFile) {
    return NewError(
        error::InvalidStream{mTarFilePath, error::StreamType::INPUT});
  }

  std::vector<char> buffer(CHUNK_SIZE_B);
  while (tarFile.read(buffer.data(), CHUNK_SIZE_B) || tarFile.gcount()) {
    BOOST_LEAF_AUTO(header, detail::ParseHeader(buffer));

    // Validate file path
    if (header.fileName.find("../", 0) != std::string::npos) {
      return NewError(error::InvalidContents{});
    }

    // Create new file with object contents
    std::ofstream extractedFile(header.fileName, std::ios::binary);
    if (!extractedFile)
      return NewError(
          error::InvalidStream{header.fileName, error::StreamType::OUTPUT});

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
    return NewError(error::InvalidFile{mTarFilePath});
  }

  std::ofstream tarFile(mTarFilePath, std::ios::binary);
  if (!tarFile) {
    return NewError(
        error::InvalidStream{mTarFilePath, error::StreamType::OUTPUT});
  }

  std::array<char, CHUNK_SIZE_B> buffer{0x00};
  for (auto const &filePath : filePaths) {
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile)
      return NewError(error::InvalidStream{filePath, error::StreamType::INPUT});

    // Extract file information
    struct stat fileInfo;
    if (stat(filePath.data(), &fileInfo) != 0) {
      return NewError(error::InvalidFile{filePath});
    }

    common::ObjectHeader header{};
    header.fileName = filePath;
    header.fileSize = fileInfo.st_size;
    header.fileMode = fileInfo.st_mode;
    header.userID = fileInfo.st_uid;
    header.groupID = fileInfo.st_gid;

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
