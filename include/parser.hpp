#pragma once

#include <string>
#include <vector>

#include "common.hpp"

namespace cc::tar {

class Parser {
public:
  Parser() = default;

  [[nodiscard]] std::vector<common::TarHeader>
  ParseFileHeaders(std::string fileName);

private:
  static const size_t CHUNK_SIZE_B = 512;
};

} // namespace cc::tar
