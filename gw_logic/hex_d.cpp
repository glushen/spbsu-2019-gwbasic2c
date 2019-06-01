#include "core/to_string.cpp"
std::string hex$_d(double value) {
    assert(-32768.5 < value && value < 65535.5, OVERFLOW);
    if (value < 0) value += 65536;
    return to_string("%X", lround(value));
}