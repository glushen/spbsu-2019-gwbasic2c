#include "core.cpp"
void print(const std::vector<std::string>& values) {
    for (auto& value : values) {
        std::cout << value;
    }
    std::cout.flush();
}