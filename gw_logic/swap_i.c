#include "core/core.c"
void swap_i(gw_int* a, gw_int* b) {
    gw_int temp = *a;
    *a = *b;
    *b = temp;
}
