#include "core.cpp"
double to_double_or_nan(std::string string) {
    for (char& c : string) {
        if (c == 'd' || c == 'D') {
            c = 'e';
        }
    }
    const char* str = string.c_str();
    char* end;
    double value = strtod(string.c_str(), &end);
    if (-1.7e38 <= value && value <= 1.7e38 && str != end) {
        return value;
    } else {
        return NAN;
    }
}