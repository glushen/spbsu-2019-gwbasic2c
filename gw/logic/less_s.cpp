#include "../core/bool.cpp"
gw_int less_s(const std::string& a, const std::string& b) {
    return to_bool(a < b);
}