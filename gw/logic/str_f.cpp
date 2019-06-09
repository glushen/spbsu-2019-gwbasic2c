#include "../core/to_string.cpp"
std::string str$_f(float value) {
    return to_string("%.7G", value);
}