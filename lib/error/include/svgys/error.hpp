#pragma once

#include "boost/leaf.hpp"
#include <cstdlib>

namespace svgys::error {

/**
 * @brief container containing either value of type T or error object
 */
template <typename T>
using Result = boost::leaf::result<T>;

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
template <class... Item>
[[nodiscard]] inline auto NewError(Item &&...p_item) {
  return boost::leaf::new_error(std::forward<Item>(p_item)...);
}

} // namespace svgys::error
