#include "../core/core.cpp"
gw_int instr(gw_int start, const std::string& haystack, const std::string& needle) {
    start--;
    assert(0 <= start && start < 255, ILLEGAL_FUNCTION_CALL);
    int found = haystack.find(needle, start);
    return found != std::string::npos ? found + 1 : 0;
}
