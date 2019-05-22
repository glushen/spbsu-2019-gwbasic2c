#include "core.c"
void collect_garbage_if_needed(void) {
    // TODO
}
char* new_string(gw_int length) {
    static int saved_line_number = -1;
    if (saved_line_number != line_number) {
        collect_garbage_if_needed();
        saved_line_number = line_number;
    }
    assert(0 <= length, ILLEGAL_FUNCTION_CALL);
    assert(length <= 255, STRING_TOO_LONG);
    char* result = malloc(length + 1);
    result[length] = 0;
    return result;
}
char* to_string(const char* format, ...) {
    va_list arguments;

    va_start(arguments, format);
    char* result = new_string(vsnprintf(NULL, 0, format, arguments));
    va_end(arguments);

    va_start(arguments, format);
    vsprintf(result, format, arguments);
    va_end(arguments);

    return result;
}