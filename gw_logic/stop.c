#include "end.c"
void stop(void) {
    printf("Break in line %d\n", line_number);
    end();
}
