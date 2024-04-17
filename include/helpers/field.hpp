#pragma once

#include "error.hpp"
#include <span>

namespace cc::tar::helpers {

template <typename T>
concept FieldValueType = requires(std::span<char> span) {
  typename T::value_type;
  {
    T::Serialise(std::declval<typename T::value_type>(), span)
  } -> std::same_as<Status>;
  { T::Parse(span) } -> std::same_as<Result<typename T::value_type>>;
};

template <typename T>
concept FieldType = std::unsigned_integral<decltype(T::offset)> &&
                    std::unsigned_integral<decltype(T::size)> &&
                    requires { typename T::value_type; };

/**
 * @brief Represents a field in the tar object header
 * @tparam Offset the offset of the field with respect to the start of the
 * header buffer
 * @tparam Size the size of the field
 * @tparam Type the wrapper type of the data contained within the field
 */
template <std::uint16_t Offset, std::uint16_t Size, FieldValueType Type>
struct Field {
  using field_type = Type;
  using value_type = Type::value_type;
  static constexpr auto offset = Offset;
  static constexpr auto size = Size;
};

/**
 * @brief Read the value of the specified field from the tar object header
 * buffer
 * @tparam Field the field of which the value should be read
 * @param buffer the buffer that contains the tar object header
 * @returns the value of the field if valid, 'NewError{}' otherwise
 */
template <FieldType Field>
[[nodiscard]] Result<typename Field::value_type> Read(std::span<char> buffer) {
  std::span<char> fieldBuffer(&buffer[Field::offset], Field::size);
  return Field::field_type::Parse(fieldBuffer);
}

/**
 * @brief Write the provided value to the specified field of the tar object
 * header buffer
 * @tparam Field the field to which the value should be written
 * @param value the value to be written
 * @param buffer the buffer that represents the tar object header
 * @return 'Success()' if no error were encountered, 'NewError{}' otherwise
 */
template <FieldType Field>
Status Write(typename Field::value_type value, std::span<char> buffer) {
  std::span<char> fieldBuffer(&buffer[Field::offset], Field::size);
  return Field::field_type::Serialise(value, fieldBuffer);
}

} // namespace cc::tar::helpers
