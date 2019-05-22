#include <cstdarg>
#include "util.h"

using std::string;

string util::filename_by_path(const string& path) {
    int slash_index = path.find_last_of("/\\");
    int substr_start = (slash_index != string::npos ? slash_index + 1 : 0);
    int dot_index = path.find_last_of('.');
    int substr_end = (dot_index != string::npos && dot_index >= substr_start ? dot_index : path.size());
    return path.substr(substr_start, substr_end - substr_start);
}

string util::escape(const string& str) {
    string result;
    result.reserve(str.size());

    for (char c : str) {
        switch (c) {
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\"':
                result += "\\\"";
                break;
            default:
                result += c;
                break;
        }
    }

    return result;
}

string util::to_string(const char* format, ...) {
    va_list arguments;

    va_start(arguments, format);
    char* result_chars = new char[vsnprintf(nullptr, 0, format, arguments) + 1];
    va_end(arguments);

    va_start(arguments, format);
    vsprintf(result_chars, format, arguments);
    va_end(arguments);

    string result_string(result_chars);
    delete[] result_chars;
    return result_string;
}
