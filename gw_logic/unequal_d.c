#include "core/bool.c"
gw_int unequal_d(double a, double b) {
    return to_bool(a != b);
}