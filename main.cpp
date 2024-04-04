#include <iostream>

#include "error.hpp"
#include "parser.hpp"

namespace cc::tar {

std::ostream &operator<<(std::ostream &os, const common::ObjectHeader &header) {
  os << header.fileName << " " << header.fileSize << "B" << std::endl;
  return os;
}

// https://cplusplus.com/doc/tutorial/files/
extern "C" int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " [options] <filename>\n";
    return InvalidFile::CODE;
  }

  std::string command{argv[1]};
  if (command.compare("-tf") == 0) {
    boost::leaf::try_handle_all(
        [&]() -> Status {
          Parser parser(argv[2]);
          BOOST_LEAF_AUTO(contents, parser.ListContents());
          for (auto const &content : contents) {
            std::cout << content;
          }
          std::cout << std::endl;
          return Success();
        },
        [&]() -> void {
          // Improve error handling
          std::cout << "Unexpected error occured!" << std::endl;
        });

  } else if (command.compare("-xf") == 0) {
    boost::leaf::try_handle_all(
        [&]() -> Status {
          Parser parser(argv[2]);
          return parser.ExtractContents();
        },
        [&]() -> void {
          // Improve error handling
          std::cout << "Unexpected error occured!" << std::endl;
        });
  }
  // Extract to disk from the archive.  If a file with the same name appears
  // more than once in the archive, each copy will be extracted, with later
  // copies overwriting (replacing) earlier copies.  The long option form is
  // --extract.

  return 0;
}

} // namespace cc::tar
