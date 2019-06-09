#include "../core/core.cpp"
float rnd(gw_int gen_next) {
    static float result = -1;
    if (gen_next || result == -1) {
        result = (float) rand() / (((float) RAND_MAX) + 1);
    }
    return result;
}
