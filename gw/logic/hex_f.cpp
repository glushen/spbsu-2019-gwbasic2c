#include "../core/to_string.cpp"
std::string hex$_f(float value) {
    assert(-32768.5f < value && value < 65535.5f, OVERFLOW);
    if (value < 0) value += 65536;
    return to_string("%X", lroundf(value));
}