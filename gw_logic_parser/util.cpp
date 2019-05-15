#include "util.h"

using std::string;

string filename_by_path(const string& path) {
    int slash_index = path.find_last_of("/\\");
    int substr_start = (slash_index != string::npos ? slash_index + 1 : 0);
    int dot_index = path.find_last_of('.');
    int substr_end = (dot_index != string::npos && dot_index >= substr_start ? dot_index : path.size());
    return path.substr(substr_start, substr_end - substr_start);
}

string escape(const string& str) {
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
