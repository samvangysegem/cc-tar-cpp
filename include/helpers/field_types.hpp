#pragma once

#include <charconv>
#include <cstdint>
#include <span>
#include <string>

#include "error.hpp"

namespace cc::tar::helpers {

struct String_t {
  using value_type = std::string;

  static Status Serialise(value_type value, std::span<char> buffer) {
    if (value.size() + 1 > buffer.size())
      return NewError(InvalidConversion{});
    std::copy_n(value.c_str(), value.size() + 1, buffer.begin());
    return Success();
  }

  static Result<value_type> Parse(std::span<char> buffer) {
    return {{buffer.data()}};
  }
};

struct Octal_t {
  using value_type = std::uint64_t;

  static Status Serialise(value_type value, std::span<char> buffer) {
    auto [ptr, ec] =
        std::to_chars(buffer.data(), buffer.data() + buffer.size(), value, 8);
    if (ec != std::errc()) {
      return NewError(InvalidConversion{});
    }
    return Success();
  }

  static Result<value_type> Parse(std::span<char> buffer) {
    value_type result{};
    auto [ptr, ec] = std::from_chars(buffer.data(),
                                     buffer.data() + buffer.size(), result, 8);
    if (ec != std::errc()) {
      return NewError(InvalidConversion{});
    }
    return {result};
  }
};

template <typename T>
concept EnumClassConvertibleToCharType =
    std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, char> &&
    (!std::is_convertible_v<T, int>); // Enum class requirement

template <EnumClassConvertibleToCharType T> struct EnumClass_t {
  using value_type = T;

  static Status Serialise(value_type value, std::span<char> buffer) {
    buffer[0] = static_cast<char>(value);
    return Success();
  }

  static Result<value_type> Parse(std::span<char> buffer) {
    value_type result = static_cast<value_type>(buffer[0]);
    return {result};
  }
};

} // namespace cc::tar::helpers
