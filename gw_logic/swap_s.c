void swap_s(const char** a, const char** b) {
    const char* temp = *a;
    *a = *b;
    *b = temp;
}
