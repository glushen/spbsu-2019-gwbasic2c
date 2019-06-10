#include "../core/core.cpp"
std::string input$(gw_int count) {
    assert(1 <= count && count <= 255, ILLEGAL_FUNCTION_CALL);
    char* chars = new char[count+1];
    std::memset(chars, 0, count+1);
    std::cin.read(chars, count);
    std::string result(chars);
    delete[] chars;
    return result;
}