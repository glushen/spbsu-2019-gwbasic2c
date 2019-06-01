#include "core/core.cpp"
gw_int asc(const std::string& string) {
    assert(!string.empty(), ILLEGAL_FUNCTION_CALL);
    return string[0] >= 0 ? string[0] : string[0] + 256;
}