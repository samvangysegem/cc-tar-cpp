#include "svgys/program_options.hpp"
#include "svgys/error.hpp"

#include <charconv>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>

namespace svgys::program_options {

bool OptionsValueMap::Contains(std::string flag) const {
  return mMap.contains(flag);
}

template <>
Result<int> OptionsValueMap::AtAs<int>(std::string flag) const {
  if (mMap.at(flag).size() != 1)
    return NewError(error::InvalidArgs{});
  auto value = mMap.at(flag)[0];

  int result{};
  auto [ptr, ec] =
      std::from_chars(value.data(), value.data() + value.size(), result);
  if (ec != std::errc())
    return NewError(error::InvalidArgs{});

  return {result};
}

template <>
Result<std::vector<std::string>>
OptionsValueMap::AtAs<std::vector<std::string>>(std::string flag) const {
  return {mMap.at(flag)};
}

template <>
Result<std::string> OptionsValueMap::AtAs<std::string>(std::string flag) const {
  if (mMap.at(flag).size() != 1)
    return NewError(error::InvalidArgs{});
  return {mMap.at(flag)[0]};
}

OptionsParser::OptionsBuilder &
OptionsParser::OptionsBuilder::operator()(std::string flag,
                                          std::string description) {
  mParser.mOptions[flag] = {.arguments = "", .description = description};
  return *this;
}

OptionsParser::OptionsBuilder &OptionsParser::OptionsBuilder::operator()(
    std::string flag, std::string arguments, std::string description) {
  mParser.mOptions[flag] = {.arguments = arguments, .description = description};
  return *this;
}

OptionsParser::OptionsBuilder OptionsParser::AddOptions() {
  return OptionsParser::OptionsBuilder(*this);
}

Result<OptionsValueMap> OptionsParser::Parse(int argc,
                                             const char *argv[]) const {
  std::map<std::string, std::vector<std::string>> valueMap;

  uint16_t index = 0;
  std::string lastKey{};
  while (index < argc) {
    std::string option{argv[index]};

    if (option.compare(0, 2, "--") == 0) {
      // Option is flag
      if (!mOptions.contains(option.substr(2))) {
        return NewError(error::InvalidFlag{});
      }
      // Flag is valid
      lastKey = option.substr(2);
      valueMap.emplace(lastKey, std::vector<std::string>());
    } else if (!lastKey.empty()) {
      // Option is flag argument
      valueMap[lastKey].push_back(option);
    }

    index++;
  }

  return {{valueMap}};
}

std::string OptionsParser::Description() const {
  std::ostringstream oss;

  for (auto const &it : mOptions) {
    oss << std::setw(45) << std::left
        << " --" + it.first + " " + it.second.arguments;
    oss << std::setw(40) << it.second.description << std::endl;
  }

  return oss.str();
}

} // namespace svgys::program_options
