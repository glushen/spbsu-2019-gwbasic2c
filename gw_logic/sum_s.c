#include "core/string.c"
const char* sum_s(const char* a, const char* b) {
    int al = strlen(a), bl = strlen(b);
    char* s = new_string(al + bl);
    strcpy(s, a);
    strcpy(s + al, b);
    return s;
}