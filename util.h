#pragma once

#include <string>
#include <fstream>
#include <sstream>

namespace util {

inline std::string read_file(const std::string &file_path) {
  std::stringstream ss;
  std::ifstream fs(file_path);
  ss << fs.rdbuf();
  return ss.str();
}

template<std::integral T, std::integral R>
constexpr bool within_bounds(T rank, R file) {
    return (rank >= 0 && rank < 8 && file >= 0 && file < 8);
}

} // namespace util
