#include "core/core.cpp"
#include "core/check_d.cpp"
double val(std::string string) {
    for (char& c : string) {
        if (c == 'd' || c == 'D') {
            c = 'e';
        }
    }
    return check_d(strtod(string.data(), nullptr));
}
