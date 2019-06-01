#include "core/core.cpp"
std::string string$_i(gw_int length, gw_int ascii_code) {
    assert(0 <= length && length <= 255 && 0 <= ascii_code && ascii_code <= 255, ILLEGAL_FUNCTION_CALL);
    return std::string(length, ascii_code < 128 ? ascii_code : ascii_code - 256);
}
