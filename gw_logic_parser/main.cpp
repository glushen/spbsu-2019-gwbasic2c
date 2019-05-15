#include <string>
#include <vector>
#include <set>
#include <cstdarg>
#include <cstring>
#include <regex>
#include <fstream>
#include <gw_logic_parser.h>
#include "main.h"
#include "util.h"

using namespace std;

extern FILE* yyin;

const regex SKIP_LINE_REGEX(R"(\s*#pragma\s+once\s*|\s*)");
const regex INCLUDE_REGEX(R"(\s*#include\s+\"([^\"]+)\"\s*)");
const regex CORE_INCLUDE_REGEX(R"(core\/\S+)");
const regex GW_FUNCTION_NAME_REGEX(R"(([^_]+)(?:_.*)?)");

map<string, CoreFileData> CORE_FILES_DATA;
map<string, LogicFileData> LOGIC_FILES_DATA;
map<string, vector<string>> LOGIC_FILES_BY_FUNCTION_NAME;

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
    CORE_FILES_DATA[filename_by_path(path)] = data;
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
    LOGIC_FILES_DATA[filename_by_path(OPENED_LOGIC_FILE_PATH)] = data;

    smatch function_name_match;
    regex_match(data.function_name, function_name_match, GW_FUNCTION_NAME_REGEX);
    LOGIC_FILES_BY_FUNCTION_NAME[function_name_match[1]].push_back(filename_by_path(OPENED_LOGIC_FILE_PATH));
}

void print_core_file(const string& name) {
    static set<string> printed;
    if (printed.count(name) > 0) return;
    printed.insert(name);

    auto& data = CORE_FILES_DATA[name];

    for (auto& path : data.include_path_list) {
        print_core_file(filename_by_path(path));
    }

    cout << "    CoreFile core_" << name << " = { ";
    cout << "\"" << name << "\", ";

    cout << "{ ";
    bool first = true;
    for (auto& path : data.include_path_list) {
        if (first) {
            first = false;
        } else {
            cout << ", ";
        }
        cout << "&core_" << filename_by_path(path);
    }
    cout << " }, ";

    cout << "\"" << escape(data.generated_code) << "\"";
    cout << " };" << endl;
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
            core_dependencies.push_back(filename_by_path(path));
        } else {
            string filename = filename_by_path(path);
            print_logic_file(filename);
            logic_dependencies.push_back(filename);
        }
    }

    cout << "    LogicFile " << name << " = { ";
    cout << "\"" << data.function_name << "\", ";

    cout << "{ ";
    bool first = true;
    for (auto& filename : core_dependencies) {
        if (first) {
            first = false;
        } else {
            cout << ", ";
        }
        cout << "&core_" << filename;
    }
    cout << " }, ";

    cout << "{ ";
    first = true;
    for (auto& path : logic_dependencies) {
        if (first) {
            first = false;
        } else {
            cout << ", ";
        }
        cout << '&' << filename_by_path(path);
    }
    cout << " }, ";

    cout << data.return_type << ", ";

    cout << "{ ";
    first = true;
    for (auto& argument_type : data.argument_type_list) {
        if (first) {
            first = false;
        } else {
            cout << ", ";
        }
        cout << argument_type;
    }
    cout << " }, ";

    cout << "\"" << escape(data.generated_code) << "\"";
    cout << " };" << endl;
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
    if (freopen(arguments[index], "w", stdout) == nullptr) {
        cerr << "Cannot open file " << arguments[index] << endl;
        exit(EXIT_FAILURE);
    }

    cout << R"(#include <map>
#include <vector>
#include <string>
using std::map, std::vector, std::string;

namespace gw_logic {
    enum Type {
        INT, FLOAT, DOUBLE, STRING, INT_PTR, FLOAT_PTR, DOUBLE_PTR, STRING_PTR, VOID
    };

    struct CoreFile {
        string name;
        vector<CoreFile*> core_dependencies;
        string code;
    };

    struct LogicFile {
        string name;
        vector<CoreFile*> core_dependencies;
        vector<LogicFile*> logic_dependencies;
        Type return_type;
        vector<Type> argument_type_list;
        string code;
    };

)";

    for (auto& name_and_data : CORE_FILES_DATA) {
        print_core_file(name_and_data.first);
    }

    cout << endl;

    for (auto& name_and_data : LOGIC_FILES_DATA) {
        print_logic_file(name_and_data.first);
    }

    cout << endl;

    cout << "    map<string, vector<LogicFile*>> LOGIC_FILES_BY_FUNCTION_NAME = {" << endl;

    int first = true;
    for (auto& name_and_filenames : LOGIC_FILES_BY_FUNCTION_NAME) {
        if (first) {
            first = false;
        } else {
            cout << "," << endl;
        }
        cout << "        { \"" << name_and_filenames.first << "\", ";

        cout << "{ ";
        int inner_first = true;
        for (auto& filename : name_and_filenames.second) {
            if (inner_first) {
                inner_first = false;
            } else {
                cout << ", ";
            }
            cout << '&' << filename;
        }
        cout << " }";

        cout << " }";
    }

    cout << endl << "    };" << endl;

    cout << '}' << endl;
}

void yyerror(const char* str, ...) {
    va_list ap;
    va_start(ap, str);
    fprintf(stderr, "Error: ");
    vfprintf(stderr, str, ap);
    fprintf(stderr, " in file %s\n", OPENED_LOGIC_FILE_PATH.c_str());
    exit(EXIT_FAILURE);
}
