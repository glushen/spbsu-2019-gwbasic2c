#include "core/bool.c"
gw_int leq_s(const char* a, const char* b) {
    return to_bool(strcmp(a, b) <= 0);
}