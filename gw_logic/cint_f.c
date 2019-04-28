#include "core/core.c"
gw_int cint_f(float value) {
    assert(-32768.5f < value && value < 32767.5f, OVERFLOW);
    return lroundf(value);
}