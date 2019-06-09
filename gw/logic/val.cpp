#include "../core/to_double_or_nan.cpp"
#include "../core/check_d.cpp"
double val(const std::string& string) {
    return check_d(to_double_or_nan(string));
}
