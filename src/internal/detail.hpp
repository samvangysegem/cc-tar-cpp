#pragma once

#include <span>

#include "common.hpp"

namespace cc::tar::detail {

common::TarHeader ParseHeader(std::span<char> buffer);

} // namespace cc::tar::detail
