#include "core/core.c"
gw_int instr(gw_int start, const char* haystack, const char* needle) {
    start--;
    assert(0 <= start && start < 255, ILLEGAL_FUNCTION_CALL);
    if (start >= strlen(haystack)) {
        return 0;
    }
    char* found = strstr(haystack + start, needle);
    return found != NULL ? (found - haystack) + 1 : 0;
}
