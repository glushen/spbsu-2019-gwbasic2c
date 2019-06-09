#include "core/core.cpp"
void lineinput(const std::string& prompt, std::string& result) {
    std::cout << prompt;
    std::cin >> result;
    if (result.size() > 255) {
        result.resize(255);
    }
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
}