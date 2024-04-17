#pragma once

#include <cstdint>
#include <span>

#include "common.hpp"
#include "error.hpp"

namespace cc::tar::detail {

[[nodiscard]] Result<common::ObjectHeader> ParseHeader(std::span<char> buffer);

[[nodiscard]] Status SerialiseHeader(common::ObjectHeader const &header,
                                     std::span<char> buffer);

} // namespace cc::tar::detail
