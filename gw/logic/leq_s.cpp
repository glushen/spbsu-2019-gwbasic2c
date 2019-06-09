#include "../core/boolean.cpp"
gw_int leq_s(const std::string& a, const std::string& b) {
    return to_bool(a <= b);
}