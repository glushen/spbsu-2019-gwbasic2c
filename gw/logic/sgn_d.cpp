#include "../core/core.cpp"
gw_int sgn_d(double value) {
    return value < 0 ? -1 : (value > 0 ? 1 : 0);
}
