#include "core/core.c"
double log_d(double value) {
    assert(value > 0, ILLEGAL_FUNCTION_CALL);
    return log(value);
}
