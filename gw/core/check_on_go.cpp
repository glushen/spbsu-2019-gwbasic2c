#include "core.cpp"
int check_on_go(int value) {
    assert(0 <= value && value <= 255, ILLEGAL_FUNCTION_CALL);
    return value;
}