#include "cint_d.cpp"
gw_int idiv_d(double a, double b) {
    gw_int a_i = cint_d(a), b_i = cint_d(b);
    assert(b_i != 0, DIVISION_BY_ZERO);
    return a_i / b_i;
}
