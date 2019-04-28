#include "core/string.c"
const char* str$_d(double value) {
    char* result = to_string("%.16lG", value);
    char* e_ptr = strchr(result, 'E');
    if (e_ptr != NULL) *e_ptr = 'D';
    return result;
}