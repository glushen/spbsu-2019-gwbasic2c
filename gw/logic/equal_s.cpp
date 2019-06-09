#include "../core/boolean.cpp"
gw_int equal_s(const std::string& a, const std::string& b) {
    return to_bool(a == b);
}