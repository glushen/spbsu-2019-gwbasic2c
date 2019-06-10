#include "../core/core.cpp"
float rnd(double gen_next) {
    static float result = -1;
    if (gen_next > 0 || result == -1) {
        result = (float) rand() / (((float) RAND_MAX) + 1);
    }
    return result;
}
