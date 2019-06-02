#pragma once
#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#define gw_int short

int line_number = 0;
bool print_each_line_number = false;

const char* ILLEGAL_FUNCTION_CALL = "Illegal function call";
const char* OVERFLOW = "Overflow";
const char* STRING_TOO_LONG = "String too long";
const char* DIVISION_BY_ZERO = "Division by zero";
const char* DUPLICATE_DEFINITION = "Duplicate Definition";
const char* MISSING_OPERAND = "Missing operand";
const char* SUBSCRIPT_OUT_OF_RANGE = "Subscript out of range";

void assert(bool condition, const char* message) {
    if (!condition) {
        printf("%s in %d\n", message, line_number);
        exit(EXIT_FAILURE);
    }
}
void set_line(int new_line_number) {
    line_number = new_line_number;
    if (print_each_line_number) printf("[%d]", line_number);
}