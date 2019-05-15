#include "core/string.c"
const char* oct$_d(double value) {
    assert(-32768.5 < value && value < 65535.5, OVERFLOW);
    if (value < 0) value += 65536;
    return to_string("%o", lround(value));
}