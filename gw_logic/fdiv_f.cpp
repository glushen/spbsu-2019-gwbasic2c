#include "core/check_f.cpp"
float fdiv_f(float a, float b) {
    assert(b != 0, DIVISION_BY_ZERO);
    return check_f(a / b);
}