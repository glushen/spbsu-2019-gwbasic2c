#pragma once

#include <vector>
#include <string>

struct CoreFileData {
    std::vector<std::string> include_path_list;
    std::string generated_code;
};

struct LogicFileData {
    std::vector<std::string> include_path_list;
    std::string return_type;
    std::string function_name;
    std::vector<std::string> argument_type_list;
    int signature_offset_in_file;
    std::string generated_code;
};

void handle_function_signature(LogicFileData&& signature);
