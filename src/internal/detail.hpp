#pragma once

#include <span>

#include "common.hpp"

namespace cc::tar::detail {

common::ObjectHeader ParseHeader(std::span<char> buffer);

} // namespace cc::tar::detail
