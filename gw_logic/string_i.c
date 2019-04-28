#include "core/string.c"
const char* string$_i(gw_int length, gw_int ascii_code) {
    assert(0 <= length && length <= 255 && 0 <= ascii_code && ascii_code <= 255, ILLEGAL_FUNCTION_CALL);
    char* result = new_string(length);
    memset(result, ascii_code, length);
    return result;
}
