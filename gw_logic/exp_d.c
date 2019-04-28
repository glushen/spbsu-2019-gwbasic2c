#include "core/core.c"
double exp_d(double value) {
    assert(value < 88.02969, OVERFLOW);
    return exp(value);
}