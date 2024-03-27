#include "parser.hpp"

#include <fstream>
#include <iostream>

#include "detail.hpp"

namespace cc::tar {

std::vector<common::TarHeader> Parser::ParseFileHeaders(std::string fileName) {
  // THIS NEEDS TO BE FIXED
  if (fileName.find(".tar") == std::string::npos) {
    std::cerr << fileName << " has invalid file type! \n";
    return {};
  }

  std::ifstream tarFile(fileName, std::ios::binary);
  if (!tarFile) {
    std::cerr << "Cannot open file: " << fileName << std::endl;
    return {}; // ToDo: Replace with fitting error code
  }

  std::vector<common::TarHeader> output;
  std::vector<char> buffer(CHUNK_SIZE_B);

  while (tarFile.read(buffer.data(), CHUNK_SIZE_B) || tarFile.gcount()) {
    std::cout << "Read " << tarFile.gcount() << " bytes of Tarfile header"
              << std::endl;

    auto header = detail::ParseHeader(buffer);
    output.push_back(header);

    auto fileSizeChunks = header.fileSize / CHUNK_SIZE_B + 1;
    tarFile.seekg(fileSizeChunks * CHUNK_SIZE_B, std::ios_base::cur);
  }

  tarFile.close();

  return output;
}

} // namespace cc::tar
