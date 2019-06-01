#include "core/core.cpp"
gw_int cint_d(double value) {
    assert(-32768.5 < value && value < 32767.5, OVERFLOW);
    return lround(value);
}