#include "core/string.c"
#include "core/min_i.c"
const char* mid$_replace(const char* string, gw_int start, gw_int count) {
    start--;
    assert(0 <= start && start < 255 && 0 <= count && count <= 255, ILLEGAL_FUNCTION_CALL);
    gw_int str_len = strlen(string);
    start = min_i(start, str_len);
    count = min_i(count, str_len - start);
    char* result = new_string(str_len);
    strncpy(result, string, str_len);
    strncpy(result, string + start, count);
    return result;
}