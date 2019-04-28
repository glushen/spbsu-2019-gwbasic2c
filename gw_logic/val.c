#include "core/string.c"
#include "core/check_d.c"
double val(const char* string) {
    if (strchr(string, 'd') != NULL || strchr(string, 'D') != NULL) {
        char* mut_string = new_string(strlen(string));
        strcpy(mut_string, string);
        char* d_ptr = strchr(mut_string, 'd');
        if (d_ptr != NULL) *d_ptr = 'e';
        d_ptr = strchr(mut_string, 'D');
        if (d_ptr != NULL) *d_ptr = 'E';
        string = mut_string;
    }
    return check_d(strtod(string, NULL));
}
