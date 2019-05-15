#pragma once

#include <vector>
#include <string>

using std::vector, std::string;

struct CoreFileData {
    vector<string> include_path_list;
    string generated_code;
};

struct LogicFileData {
    vector<string> include_path_list;
    string return_type;
    string function_name;
    vector<string> argument_type_list;
    int signature_offset_in_file;
    string generated_code;
};

void handle_function_signature(LogicFileData&& signature);
