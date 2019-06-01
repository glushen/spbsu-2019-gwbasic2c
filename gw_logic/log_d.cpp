#include "core/core.cpp"
double log_d(double value) {
    assert(value > 0, ILLEGAL_FUNCTION_CALL);
    return log(value);
}
