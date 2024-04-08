#pragma once

#include "boost/leaf.hpp"

namespace cc::tar {

/**
 * @brief container containing either value of type T or error object
 */
template <typename T> using Result = boost::leaf::result<T>;

/**
 * @brief container containing either nothing or an error object
 */
using Status = Result<void>;

/**
 * @brief function for returning a successful result, with improved readability
 */
[[nodiscard]] inline Status Success() { return {}; }

/**
 * @brief function for returning an error, with improved readability
 */
template <class... Item> [[nodiscard]] inline auto NewError(Item &&...p_item) {
  return boost::leaf::new_error(std::forward<Item>(p_item)...);
}

/**
 * Error objects
 */
struct InvalidArgs {
  static constexpr int CODE = -1;
};
struct InvalidFile {
  static constexpr int CODE = -2;
};
struct InvalidStream {
  static constexpr int CODE = -3;
};
struct InvalidContents {
  static constexpr int CODE = -4;
  std::string fileName;
  std::string description;
};
struct UnexpectedError {
  static constexpr int CODE = -99;
};

} // namespace cc::tar
