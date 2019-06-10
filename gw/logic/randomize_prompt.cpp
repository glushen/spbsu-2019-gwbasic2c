#include "../core/input.cpp"
void randomize_prompt() {
    gw_int seed;
    input("Random number seed (-32768 to 32767)? ", {seed});
    srand(seed + 32768u);
}
