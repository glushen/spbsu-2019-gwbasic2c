#include "core/core.c"
void randomize(gw_int seed) {
    srand(seed + 32768u);
}
