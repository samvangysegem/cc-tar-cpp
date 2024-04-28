#pragma once

#include <map>
#include <string>
#include <vector>

#include "svgys/error.hpp"

namespace svgys::program_options {

// Point of improvement
// - Pass type information for each flag
// - Use type information during parsing for error detection
// - Valuemap should return correct type from the start
// - Allow flag abbreviations

namespace error {
struct InvalidFlag {};
struct InvalidArgs {};
} // namespace error

using namespace svgys::error;

struct OptionDescription {
  std::string arguments;
  std::string description;
};

class OptionsValueMap {
  using MapType = std::map<std::string, std::vector<std::string>>;

public:
  OptionsValueMap(MapType map) : mMap(std::move(map)) {}

  [[nodiscard]] bool Contains(std::string flag) const;

  template <typename T>
  [[nodiscard]] Result<T> AtAs(std::string flag) const;

private:
  MapType mMap;
};

class OptionsParser {
public:
  class OptionsBuilder {
  public:
    explicit OptionsBuilder(OptionsParser &parser) : mParser(parser) {}

    OptionsBuilder &operator()(std::string flag, std::string description);

    OptionsBuilder &operator()(std::string flag, std::string arguments,
                               std::string description);

  private:
    OptionsParser &mParser;
  };

public:
  OptionsParser() = default;

  [[nodiscard]] OptionsBuilder AddOptions();

  [[nodiscard]] Result<OptionsValueMap> Parse(int argc,
                                              const char *argv[]) const;

  [[nodiscard]] std::string Description() const;

private:
  std::map<std::string, OptionDescription> mOptions{};
};

} // namespace svgys::program_options
