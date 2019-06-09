#include "core.cpp"
float check_f(float value) {
    assert(-1.7e38f <= value && value <= 1.7e38f, OVERFLOW);
    return value;
}