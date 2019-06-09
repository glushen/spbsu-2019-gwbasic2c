#include "core.cpp"
double check_d(double value) {
    assert(-1.7e38 <= value && value <= 1.7e38, OVERFLOW);
    return value;
}