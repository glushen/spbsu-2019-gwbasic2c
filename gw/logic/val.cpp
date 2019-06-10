#include "../core/to_double_or_nan.cpp"
#include "../core/check_d.cpp"
double val(const std::string& string) {
    double value = to_double_or_nan(string);
    return value == value ? check_d(value) : 0;
}
