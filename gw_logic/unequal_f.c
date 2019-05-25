#include "core/bool.c"
gw_int unequal_f(float a, float b) {
    return to_bool(a != b);
}