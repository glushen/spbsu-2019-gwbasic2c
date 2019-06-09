#include "../core/core.cpp"
std::string sum_s(const std::string& a, const std::string& b) {
    assert(a.size() + b.size() <= 255, STRING_TOO_LONG);
    return a + b;
}