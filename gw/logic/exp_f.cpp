#include "../core/core.cpp"
float exp_f(float value) {
    assert(value < 88.02969f, OVERFLOW);
    return expf(value);
}