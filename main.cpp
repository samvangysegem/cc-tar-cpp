#include <iostream>

#include "boost/leaf/error.hpp"
#include "boost/leaf/handle_errors.hpp"
#include "error_code.hpp"
#include "file_handler.hpp"
#include "svgys/error.hpp"
#include "svgys/program_options.hpp"

namespace cc::tar {

std::ostream &operator<<(std::ostream &os, const common::ObjectHeader &header) {
  os << header.fileName << " " << header.fileSize << "B" << std::endl;
  return os;
}

extern "C" int main(int argc, const char *argv[]) {
  using namespace svgys::program_options;

  OptionsParser parser{};
  parser.AddOptions()("help", "show man page")("list", "<tar_filepath>",
                                               "show contents of tar archive")(
      "create", "<tar_filepath> [filepaths...]",
      "create tar archive")("extract", "<tar_filepath>", "extract tar archive");

  return boost::leaf::try_handle_all(
      [&]() -> Result<int> {
        BOOST_LEAF_AUTO(options, parser.Parse(argc, argv));

        if (options.Contains("help")) {
          std::cout << "Usage:\n";
          std::cout << parser.Description();
        } else if (options.Contains("list")) {
          BOOST_LEAF_AUTO(fileName, options.AtAs<std::string>("list"));

          FileHandler handler(fileName);
          BOOST_LEAF_AUTO(contents, handler.ListContents());
          for (auto const &content : contents) {
            std::cout << content;
          }
          std::cout << std::endl;
        } else if (options.Contains("create")) {
          BOOST_LEAF_AUTO(files,
                          options.AtAs<std::vector<std::string>>("create"));
          if (files.size() < 2)
            return NewError(svgys::program_options::error::InvalidArgs{});

          FileHandler handler(files[0]);
          BOOST_LEAF_CHECK(handler.Compress({files.begin() + 1, files.end()}));
        } else if (options.Contains("extract")) {
          BOOST_LEAF_AUTO(tarFileName, options.AtAs<std::string>("extract"));

          FileHandler handler(tarFileName);
          BOOST_LEAF_CHECK(handler.Extract());
        } else {
          std::cout << "No arguments provided!\n";
          std::cout << "Usage:\n";
          std::cout << parser.Description();
        }
        return 0;
      },
      [&](svgys::program_options::error::InvalidFlag err) -> int {
        std::cout << "Invalid flag was passed!\n";
        std::cout << "Usage:\n";
        std::cout << parser.Description();
        return error::InvalidProgramArgs::CODE;
      },
      [&](svgys::program_options::error::InvalidArgs err) -> int {
        std::cout << "Invalid argument(s) were passed!\n";
        std::cout << "Usage:\n";
        std::cout << parser.Description();
        return error::InvalidProgramArgs::CODE;
      },
      [](error::InvalidFile err) -> int {
        std::cout << err.fileName << " is not a valid file or file path!\n";
        return error::InvalidFile::CODE;
      },
      [](error::InvalidStream err) -> int {
        std::cout << "Unexpected error occured while "
                  << ((err.type == error::StreamType::INPUT) ? "reading from"
                                                             : "writing to")
                  << err.fileName << "! Verify the file path is correct!\n";
        return error::InvalidStream::CODE;
      },
      [](error::InvalidContents err) -> int {
        std::cout << "tar ball contains invalid file paths!\n";
        return error::InvalidContents::CODE;
      },
      [](error::InvalidConversion err) -> int {
        std::cout << "tar ball is damaged or has invalid contents!\n";
        return error::InvalidContents::CODE;
      },
      [](error::InvalidChecksum err) -> int {
        std::cout << "tar ball is damaged: invalid checksum!\n";
        return error::InvalidContents::CODE;
      },
      []() -> int {
        std::cout << "Unexpected error occured!\n";
        return error::UnexpectedError::CODE;
      });
}

} // namespace cc::tar
