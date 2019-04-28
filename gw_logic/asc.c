#include "core/core.c"
gw_int asc(const char* string) {
    assert(string[0] != 0, ILLEGAL_FUNCTION_CALL);
    return string[0] >= 0 ? string[0] : string[0] + 256;
}