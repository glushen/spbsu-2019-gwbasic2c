#include "../core/core.cpp"
gw_int sgn_i(gw_int value) {
    return value < 0 ? -1 : (value > 0 ? 1 : 0);
}
