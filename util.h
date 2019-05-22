#pragma once
#include <string>

namespace util {
    std::string filename_by_path(const std::string& path);

    std::string escape(const std::string& str);

    std::string to_string(const char* format, ...);
}
