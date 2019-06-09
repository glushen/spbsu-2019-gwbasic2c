#include "mid.cpp"
#include "../core/max_i.cpp"
std::string right$(const std::string& string, gw_int count) {
    return mid$(string, max_i(0, string.size() - count) + 1, count);
}
