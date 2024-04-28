#pragma once

#include <string>

namespace cc::tar::error {

/**
 * Error objects
 */
struct InvalidProgramArgs {
  static constexpr int CODE = -1;
};

struct InvalidFile {
  static constexpr int CODE = -2;
  std::string fileName;
};

enum class StreamType { INPUT, OUTPUT };

struct InvalidStream {
  static constexpr int CODE = -3;
  std::string fileName;
  StreamType type;
};

struct InvalidContents {
  static constexpr int CODE = -4;
};

struct InvalidBufferSize {
  static constexpr int CODE = -5;
};

struct InvalidConversion {
  static constexpr int CODE = -6;
};

struct InvalidChecksum {
  static constexpr int CODE = -7;
};

struct UnexpectedError {
  static constexpr int CODE = -99;
};

} // namespace cc::tar::error
