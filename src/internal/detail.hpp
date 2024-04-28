#pragma once

#include <cstdint>
#include <span>

#include "common.hpp"
#include "svgys/error.hpp"

namespace cc::tar::detail {
using namespace svgys::error;

[[nodiscard]] Result<common::ObjectHeader> ParseHeader(std::span<char> buffer);

[[nodiscard]] Status SerialiseHeader(common::ObjectHeader const &header,
                                     std::span<char> buffer);

} // namespace cc::tar::detail
