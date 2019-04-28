#include "string_i.c"
const char* string$_s(gw_int length, const char* string) {
    assert(string[0] != 0, ILLEGAL_FUNCTION_CALL);
    return string$_i(length, string[0]);
}
