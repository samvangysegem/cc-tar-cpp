#include "detail.hpp"

#include <span>
#include <string>

namespace cc::tar::detail {

common::TarHeader ParseHeader(std::span<char> buffer) {
  if (buffer.size_bytes() < 257)
    return {};

  common::TarHeader header{};
  header.fileName = std::string(buffer.begin(), buffer.begin() + 100);
  return header;
}

} // namespace cc::tar::detail
