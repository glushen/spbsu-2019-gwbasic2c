#include "../core/core.cpp"
gw_int eqv_(gw_int a, gw_int b) {
    return ~(a ^ b);
}