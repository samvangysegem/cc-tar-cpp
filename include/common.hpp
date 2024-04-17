#pragma once

#include "helpers/field.hpp"
#include "helpers/field_types.hpp"

namespace cc::tar::common {

using FILE_NAME = helpers::Field<0, 100, helpers::String_t>;

using FILE_MODE = helpers::Field<100, 8, helpers::Octal_t>;

using USER_ID = helpers::Field<108, 8, helpers::Octal_t>;

using GROUP_ID = helpers::Field<116, 8, helpers::Octal_t>;

using FILE_SIZE = helpers::Field<124, 12, helpers::Octal_t>;

using LAST_MODIFIED = helpers::Field<136, 12, helpers::Octal_t>;

using CHECKSUM = helpers::Field<148, 12, helpers::Octal_t>;

/**
 * @brief Set of values for \ref LINK_INDICATOR
 */
enum class LinkIndicator : char {
  NORMAL_FILE = '0',
  HARD_LINK = '1',
  SYMBOLIC_LINK = '2',
};

using LINK_INDICATOR =
    helpers::Field<156, 1, helpers::EnumClass_t<LinkIndicator>>;

using LINKED_FILE_NAME = helpers::Field<157, 100, helpers::String_t>;

// TODO Replace with better way of doing this..
// Perhaps general container with fields similar to cib
struct ObjectHeader {
  std::string fileName;
  std::uint64_t fileSize;
  std::uint64_t fileMode;

  std::uint64_t userID;
  std::uint64_t groupID;

  LinkIndicator linkIndicator;
  std::string linkedFileName;
};

} // namespace cc::tar::common
