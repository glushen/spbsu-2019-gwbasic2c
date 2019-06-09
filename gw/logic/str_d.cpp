#include "../core/to_string.cpp"
std::string str$_d(double value) {
    std::string result = to_string("%.16lG", value);
    for (char& c : result) {
        if (c == 'E') {
            c = 'D';
            break;
        }
    }
    return result;
}