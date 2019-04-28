#include "core/string.c"
const char* chr$(gw_int ascii_code) {
    assert(0 <= ascii_code && ascii_code <= 255, ILLEGAL_FUNCTION_CALL);
    char* result = new_string(1);
    result[0] = ascii_code < 128 ? ascii_code : ascii_code - 256;
    return result;
}