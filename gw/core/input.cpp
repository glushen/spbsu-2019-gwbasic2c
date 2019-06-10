#include "core.cpp"
#include "to_double_or_nan.cpp"
struct InputReceiver {
    gw_int* i = nullptr;
    float* f = nullptr;
    double* d = nullptr;
    std::string* s = nullptr;
    InputReceiver(gw_int& i): i(&i) { }
    InputReceiver(float& f): f(&f) { }
    InputReceiver(double& d): d(&d) { }
    InputReceiver(std::string& s): s(&s) { }
};
void input(const std::string& prompt, const std::vector<InputReceiver>& receivers) {
    bool firstAttempt = true;
    bool invalidInput = true;

    while (invalidInput) {
        if (firstAttempt) {
            firstAttempt = false;
        } else {
            std::cout << "?Redo from start" << std::endl;
        }
        std::cout << prompt;
        std::cout.flush();

        invalidInput = false;

        std::string input;
        std::getline(std::cin, input);

        int commaPosition = -1;

        for (auto& receiver : receivers) {
            if (commaPosition >= int(input.size())) {
                invalidInput = true;
                break;
            }

            int start = commaPosition + 1;
            commaPosition = input.find(',', start);
            if (commaPosition == std::string::npos) {
                commaPosition = input.size();
            }

            std::string value = input.substr(start, commaPosition - start);

            if (receiver.s) {
                int valueStart = input.find_first_not_of(' ');
                if (valueStart == std::string::npos) {
                    receiver.s->clear();
                } else {
                    *(receiver.s) = value.substr(valueStart, input.find_last_not_of(' ') + 1 - valueStart);
                }
            } else {
                double doubleValue = to_double_or_nan(value);
                if (doubleValue != doubleValue) /* if nan */ {
                    invalidInput = true;
                    break;
                }

                if (receiver.d) {
                    *(receiver.d) = doubleValue;
                } else if (receiver.f) {
                    *(receiver.f) = doubleValue;
                } else if (receiver.i) {
                    if (-32768.5 < doubleValue && doubleValue < 32767.5) {
                        *(receiver.i) = lround(doubleValue);
                    } else {
                        invalidInput = true;
                    }
                }
            }
        }

        if (commaPosition != -1 && commaPosition != input.size()) {
            invalidInput = true;
        }
    }
}
