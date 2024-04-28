#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "common.hpp"
#include "svgys/error.hpp"

namespace cc::tar {
using namespace svgys::error;

class FileHandler {
public:
  FileHandler(std::string tarFilePath) : mTarFilePath(tarFilePath) {}

  [[nodiscard]] bool IsValid() noexcept;

  [[nodiscard]] Result<std::vector<common::ObjectHeader>>
  ListContents() noexcept;

  [[nodiscard]] Status Extract() noexcept;

  [[nodiscard]] Status Compress(std::vector<std::string> filePaths) noexcept;

private:
  static constexpr std::uint64_t CHUNK_SIZE_B = 512;

  std::string mTarFilePath;
};

} // namespace cc::tar
