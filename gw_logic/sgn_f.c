#include "core/core.c"
gw_int sgn_f(float value) {
    return value < 0 ? -1 : (value > 0 ? 1 : 0);
}
