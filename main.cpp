#include <iostream>

#include "error.hpp"
#include "parser.hpp"

namespace cc::tar {
// https://cplusplus.com/doc/tutorial/files/
extern "C" int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [options] <filename>\n";
    return static_cast<int>(ErrorCode::INVALID_ARGS);
  }

  // Perhaps a command pattern could be useful later but for now,
  // just a hard check
  if (std::string(argv[1]).compare("-tf") == 0) {
    Parser parser{};
    auto fileHeaders = parser.ParseFileHeaders(argv[2]);
    for (const auto &header : fileHeaders) {
      std::cout << header.fileName << std::endl;
    }
  }

  return 0;
}

} // namespace cc::tar
