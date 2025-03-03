#pragma once

#include <string>
#include <fstream>
#include <sstream>

std::string read_file(const std::string& file_path) {
    std::stringstream ss;
    std::ifstream fs(file_path);
    ss << fs.rdbuf();
    return ss.str();
}
