#include "cint_f.cpp"
gw_int idiv_f(float a, float b) {
    gw_int a_i = cint_f(a), b_i = cint_f(b);
    assert(b_i != 0, DIVISION_BY_ZERO);
    return a_i / b_i;
}