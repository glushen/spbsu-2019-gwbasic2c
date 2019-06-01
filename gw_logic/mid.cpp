#include "core/core.cpp"
#include "core/min_i.cpp"
std::string mid$(const std::string& string, gw_int start, gw_int count) {
    start--;
    assert(0 <= start && start < 255 && 0 <= count && count <= 255, ILLEGAL_FUNCTION_CALL);
    gw_int str_len = string.size();
    start = min_i(start, str_len);
    count = min_i(count, str_len - start);
    return string.substr(start, count);
}
