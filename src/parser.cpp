#include "parser.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>

#include "common.hpp"
#include "detail.hpp"
#include "error.hpp"

namespace cc::tar {

bool Parser::IsValid() noexcept {
  auto validExtension =
      mFileName.find(".tar", mFileName.size() - 4) != std::string::npos;
  return validExtension;
}

Result<std::vector<common::ObjectHeader>> Parser::ListContents() noexcept {
  if (!IsValid()) {
    return NewError(InvalidFile{});
  }

  std::ifstream tarFile(mFileName, std::ios::binary);
  if (!tarFile) {
    return NewError(InvalidStream{});
  }

  std::vector<common::ObjectHeader> output{};
  std::vector<char> buffer(CHUNK_SIZE_B);
  while (tarFile.read(buffer.data(), CHUNK_SIZE_B) || tarFile.gcount()) {
#ifdef DEBUG
    std::cout << "Read " << tarFile.gcount() << " bytes of Tarfile header"
              << std::endl;
#endif

    auto header = detail::ParseHeader(buffer);
    if (header.fileName.empty() || header.fileSize == 0)
      break;
    output.push_back(header);

    auto fileSizeChunks = header.fileSize / CHUNK_SIZE_B + 1;
    tarFile.seekg(fileSizeChunks * CHUNK_SIZE_B, std::ios_base::cur);
  }

  tarFile.close();

  return {output};
}

Status Parser::ExtractContents() noexcept {
  if (!IsValid()) {
    return NewError(InvalidFile{});
  }

  std::ifstream tarFile(mFileName, std::ios::binary);
  if (!tarFile) {
    return NewError(InvalidStream{});
  }

  std::vector<char> buffer(CHUNK_SIZE_B);
  while (tarFile.read(buffer.data(), CHUNK_SIZE_B) || tarFile.gcount()) {
    // Check header validity
    auto header = detail::ParseHeader(buffer);
    if (header.fileName.empty() || header.fileSize == 0)
      break;

    // Create new file with object contents
    std::ofstream extractedFile(header.fileName, std::ios::binary);
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

} // namespace cc::tar
