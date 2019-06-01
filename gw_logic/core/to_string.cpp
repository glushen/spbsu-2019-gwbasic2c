#include "core.cpp"
std::string to_string(const char* format, ...) {
    va_list arguments;

    va_start(arguments, format);
    char* result_chars = new char[vsnprintf(nullptr, 0, format, arguments) + 1];
    va_end(arguments);

    va_start(arguments, format);
    vsprintf(result_chars, format, arguments);
    va_end(arguments);

    std::string result_string(result_chars);
    delete[] result_chars;
    return result_string;
}