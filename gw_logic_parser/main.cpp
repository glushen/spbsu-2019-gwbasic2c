#include <string>
#include <vector>
#include <set>
#include <cstdarg>
#include <cstring>
#include <regex>
#include <fstream>
#include <algorithm>
#include <gw_logic_parser.h>
#include "main.h"
#include "util.h"

using namespace std;

extern FILE* yyin;

const regex SKIP_LINE_REGEX(R"(\s*#pragma\s+once\s*|\s*)");
const regex INCLUDE_REGEX(R"(\s*#include\s+\"([^\"]+)\"\s*)");
const regex CORE_INCLUDE_REGEX(R"(\.\.\/core\/\S+)");
const regex GW_FUNCTION_NAME_REGEX(R"(([^_]+)(?:_.*)?)");

map<string, CoreFileData> CORE_FILES_DATA;
map<string, LogicFileData> LOGIC_FILES_DATA;
map<string, vector<string>> LOGIC_FILES_BY_FUNCTION_NAME;

ofstream hout;
ofstream cppout;

FILE* OPENED_LOGIC_FILE;
string OPENED_LOGIC_FILE_PATH;

void manage_core_file(const char* path) {
    ifstream fin(path);
    if (!fin.is_open()) {
        cerr << "Cannot open file " << path << endl;
        exit(EXIT_FAILURE);
    }
    CoreFileData data = {};

    for (string line; getline(fin, line);) {
        if (regex_match(line, SKIP_LINE_REGEX)) {
            continue;
        }

        smatch include_match_result;
        if (regex_match(line, include_match_result, INCLUDE_REGEX)) {
            data.include_path_list.push_back(include_match_result[1]);
            continue;
        }

        if (!data.generated_code.empty()) {
            data.generated_code += '\n';
        }
        data.generated_code += line;
    }

    fin.close();
    CORE_FILES_DATA[util::filename_by_path(path)] = data;
}

void manage_logic_file(const char* filename) {
    OPENED_LOGIC_FILE = fopen(filename, "r");
    if (OPENED_LOGIC_FILE == nullptr) {
        cerr << "Cannot open file " << filename << endl;
        exit(EXIT_FAILURE);
    }
    OPENED_LOGIC_FILE_PATH = filename;

    yyin = OPENED_LOGIC_FILE;
    int result = yyparse(); // will run handle_function_signature()
    if (result != 0) {
        cerr << "Cannot parse file " << filename << endl;
        exit(EXIT_FAILURE);
    }
}

bool gw_types_comparator(const string &a, const string &b) {
    return (a == "INT" && (b == "FLOAT" || b == "DOUBLE"))
        || (a == "FLOAT" && b == "DOUBLE");
}

void save_logic_file_by_function_name(const string& gw_function_name,
                                      const string& new_logic_filename,
                                      const LogicFileData& new_data) {
    auto& data_list = LOGIC_FILES_BY_FUNCTION_NAME[gw_function_name];

    int insert_index = 0;

    for (auto& logic_filename : data_list) {
        auto& current_data = LOGIC_FILES_DATA[logic_filename];

        if (current_data.argument_type_list.size() == new_data.argument_type_list.size()) {
            bool new_is_less = lexicographical_compare(
                    new_data.argument_type_list.begin(),
                    new_data.argument_type_list.end(),
                    current_data.argument_type_list.begin(),
                    current_data.argument_type_list.end(),
                    gw_types_comparator);

            if (new_is_less) {
                break;
            }
        }

        insert_index++;
    }

    data_list.insert(data_list.begin() + insert_index, new_logic_filename);
}

void handle_function_signature(LogicFileData&& data) {
    fclose(OPENED_LOGIC_FILE);

    ifstream fin(OPENED_LOGIC_FILE_PATH);
    if (!fin.is_open()) {
        cerr << "Cannot open file " << OPENED_LOGIC_FILE_PATH << endl;
        exit(EXIT_FAILURE);
    }

    fin.ignore(data.signature_offset_in_file);

    for (string line; getline(fin, line);) {
        if (regex_match(line, SKIP_LINE_REGEX)) {
            continue;
        }

        if (!data.generated_code.empty()) {
            data.generated_code += '\n';
        }
        data.generated_code += line;
    }

    fin.close();

    auto filename = util::filename_by_path(OPENED_LOGIC_FILE_PATH);
    LOGIC_FILES_DATA[filename] = data;

    smatch gw_function_name_match;
    regex_match(data.function_name, gw_function_name_match, GW_FUNCTION_NAME_REGEX);
    save_logic_file_by_function_name(gw_function_name_match[1], filename, data);
}

void print_core_file(const string& name) {
    static set<string> printed;
    if (printed.count(name) > 0) return;
    printed.insert(name);

    auto& data = CORE_FILES_DATA[name];

    for (auto& path : data.include_path_list) {
        print_core_file(util::filename_by_path(path));
    }

    hout << "        extern const File* " << name << ";" << endl;

    cppout << "const gw::core::File* gw::core::" << name << " = new gw::core::File({ ";
    cppout << "\"" << name << "\", ";

    cppout << "{ ";
    bool first = true;
    for (auto& path : data.include_path_list) {
        if (first) {
            first = false;
        } else {
            cppout << ", ";
        }
        cppout << "gw::core::" << util::filename_by_path(path);
    }
    cppout << " }, ";

    cppout << "\"" << util::escape(data.generated_code) << "\"";
    cppout << " });" << endl;
}

void print_logic_file(const string& name) {
    static set<string> printed;
    if (printed.count(name) > 0) return;
    printed.insert(name);

    auto& data = LOGIC_FILES_DATA[name];

    vector<string> core_dependencies;
    vector<string> logic_dependencies;

    for (auto& path : data.include_path_list) {
        if (regex_match(path, CORE_INCLUDE_REGEX)) {
            core_dependencies.push_back(util::filename_by_path(path));
        } else {
            string filename = util::filename_by_path(path);
            print_logic_file(filename);
            logic_dependencies.push_back(filename);
        }
    }

    hout << "        extern const File* " << name << ";" << endl;

    cppout << "const gw::logic::File* gw::logic::" << name << " = new gw::logic::File({ ";
    cppout << "\"" << data.function_name << "\", ";

    cppout << "{ ";
    bool first = true;
    for (auto& filename : core_dependencies) {
        if (first) {
            first = false;
        } else {
            cppout << ", ";
        }
        cppout << "gw::core::" <<  filename;
    }
    cppout << " }, ";

    cppout << "{ ";
    first = true;
    for (auto& path : logic_dependencies) {
        if (first) {
            first = false;
        } else {
            cppout << ", ";
        }
        cppout << "gw::logic::" << util::filename_by_path(path);
    }
    cppout << " }, ";

    cppout << "gw::" << data.return_type << ", ";

    cppout << "{ ";
    first = true;
    for (auto& argument_type : data.argument_type_list) {
        if (first) {
            first = false;
        } else {
            cppout << ", ";
        }
        cppout << "gw::" << argument_type;
    }
    cppout << " }, ";

    cppout << "\"" << util::escape(data.generated_code) << "\"";
    cppout << " });" << endl;
}

int main(int count, char* arguments[]) {
    int index = 1;
    
    assert(index < count);
    assert(strcmp(arguments[index], "-core") == 0);
    index++;
    
    while (index < count && strcmp(arguments[index], "-logic") != 0) {
        manage_core_file(arguments[index]);
        index++;
    }

    assert(index < count);
    assert(strcmp(arguments[index], "-logic") == 0);
    index++;

    while (index < count && strcmp(arguments[index], "-output") != 0) {
        manage_logic_file(arguments[index]);
        index++;
    }

    assert(index < count);
    assert(strcmp(arguments[index], "-output") == 0);
    index++;

    assert(index < count);
    hout.open(arguments[index]);
    index++;

    assert(index < count);
    cppout.open(arguments[index]);
    index++;
    
    assert(index == count);

    hout << R"(#pragma once
#include <map>
#include <vector>
#include <string>

namespace gw {
    using std::map, std::vector, std::string;

    enum Type {
        INT, FLOAT, DOUBLE, STRING, INT_REF, FLOAT_REF, DOUBLE_REF, STRING_REF, VOID
    };

    namespace core {
        struct File {
            string name;
            vector<const File*> coreDependencies;
            string code;
        };

)";

    cppout << "#include \"gw.h\"" << endl << endl;

    for (auto& name_and_data : CORE_FILES_DATA) {
        print_core_file(name_and_data.first);
    }

    hout << R"(    }

    namespace logic {
        struct File {
            string name;
            vector<const core::File*> coreDependencies;
            vector<const logic::File*> logicDependencies;
            Type returnType;
            vector<Type> argumentTypes;
            string code;
        };

)";

    for (auto& name_and_data : LOGIC_FILES_DATA) {
        print_logic_file(name_and_data.first);
    }

    hout << endl;
    hout << "        extern const map<string, vector<const File*>> BY_FUNCTION_NAME;" << endl;
    hout << "    }" << endl;
    hout << '}' << endl;

    cppout << endl;
    cppout << "const std::map<std::string, std::vector<const gw::logic::File*>> gw::logic::BY_FUNCTION_NAME = {" << endl;

    int first = true;
    for (auto& name_and_filenames : LOGIC_FILES_BY_FUNCTION_NAME) {
        if (first) {
            first = false;
        } else {
            cppout << "," << endl;
        }
        cppout << "    { \"" << name_and_filenames.first << "\", ";

        cppout << "{ ";
        int inner_first = true;
        for (auto& filename : name_and_filenames.second) {
            if (inner_first) {
                inner_first = false;
            } else {
                cppout << ", ";
            }
            cppout << "gw::logic::" << filename;
        }
        cppout << " }";

        cppout << " }";
    }

    cppout << endl << "};" << endl;
}

void yyerror(const char* str, ...) {
    va_list ap;
    va_start(ap, str);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, str, ap);
    fprintf(stderr, " in file %s\n", OPENED_LOGIC_FILE_PATH.c_str());
    va_end(ap);
    exit(EXIT_FAILURE);
}
