#include "string_i.cpp"
std::string string$_s(gw_int length, const std::string& string) {
    assert(!string.empty(), ILLEGAL_FUNCTION_CALL);
    return string$_i(length, string[0]);
}
