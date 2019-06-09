#include "../core/check_d.cpp"
double fdiv_d(double a, double b) {
    assert(b != 0, DIVISION_BY_ZERO);
    return check_d(a / b);
}
