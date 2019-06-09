#include "../core/core.cpp"
#include "../core/min_i.cpp"
void mid$_replace(std::string& string, gw_int start, gw_int count, const std::string& replacement) {
    start--;
    assert(0 <= start && start < 255 && 0 <= count && count <= 255, ILLEGAL_FUNCTION_CALL);
    gw_int str_len = string.size();
    start = min_i(start, str_len);
    count = min_i(min_i(count, str_len - start), replacement.size());
    string.replace(start, count, replacement);
}