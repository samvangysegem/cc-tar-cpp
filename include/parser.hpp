#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "common.hpp"
#include "error.hpp"

namespace cc::tar {

class Parser {
public:
  Parser(std::string fileName) : mFileName(fileName) {}

  [[nodiscard]] bool IsValid() noexcept;

  [[nodiscard]] Result<std::vector<common::ObjectHeader>>
  ListContents() noexcept;

  [[nodiscard]] Status ExtractContents() noexcept;

private:
  static constexpr std::uint64_t CHUNK_SIZE_B = 512;

  std::string mFileName;
};

} // namespace cc::tar
