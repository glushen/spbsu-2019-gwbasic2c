#include "mid.c"
#include "core/max_i.c"
const char* right$(const char* string, gw_int count) {
    return mid$(string, max_i(0, strlen(string) - count) + 1, count);
}
