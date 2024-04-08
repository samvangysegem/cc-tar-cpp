#include <iostream>

#include "boost/leaf/error.hpp"
#include "error.hpp"
#include "file_handler.hpp"

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
    return boost::leaf::try_handle_all(
        [&]() -> Result<int> {
          FileHandler handler(argv[2]);
          BOOST_LEAF_AUTO(contents, handler.ListContents());
          for (auto const &content : contents) {
            std::cout << content;
          }
          std::cout << std::endl;
          return {0};
        },
        [&]() -> int {
          // Improve error handling
          std::cerr << "Unexpected error occured!" << std::endl;
          return UnexpectedError::CODE;
        });

  } else if (command.compare("-xf") == 0) {
    return boost::leaf::try_handle_all(
        [&]() -> Result<int> {
          FileHandler handler(argv[2]);
          BOOST_LEAF_CHECK(handler.Extract());
          return {0};
        },
        [](InvalidContents err) -> int {
          std::cerr << err.fileName << ": " << err.description << std::endl;
          return InvalidContents::CODE;
        },
        [&]() -> int {
          // Improve error handling
          std::cerr << "Unexpected error occured!" << std::endl;
          return UnexpectedError::CODE;
        });
  } else if (command.compare("-cf") == 0) {
    if (argc < 3) {
      std::cerr << "Missing arguments! Usage: cc-tar -cf <filename> "
                   "<files_to_compress>";
      return InvalidArgs::CODE;
    }
    return boost::leaf::try_handle_all(
        [&]() -> Result<int> {
          FileHandler handler(argv[2]);
          std::vector<std::string> filePaths{};
          for (auto argsIndex = 3; argsIndex < argc; argsIndex++) {
            filePaths.emplace_back(argv[argsIndex]);
          }
          BOOST_LEAF_CHECK(handler.Compress(filePaths));
          return {0};
        },
        []() -> int {
          // Improve error handling
          std::cerr << "Unexpected error occured!" << std::endl;
          return UnexpectedError::CODE;
        });
  }

  return 0;
}

} // namespace cc::tar
