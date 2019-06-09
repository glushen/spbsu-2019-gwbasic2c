#include "../core/core.cpp"
float log_f(float value) {
    assert(value > 0, ILLEGAL_FUNCTION_CALL);
    return logf(value);
}
