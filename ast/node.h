#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <memory>
#include <gw_logic.h>
#include <cmake-build-debug/gw_logic.h>

namespace ast {
    std::string to_string(gw_logic::Type type);

    class Printable {
    public:
        virtual void print(std::ostream& stream) const = 0;
    protected:
        virtual ~Printable();
    };

    template <typename T>
    void joinAndPrint(std::ostream& stream, const std::vector<T>& values, const std::string& separator = ",") {
        bool first = true;
        for (auto& value : values) {
            if (first) {
                first = false;
            } else {
                stream << separator;
            }
            value->print(stream);
        }
    }
}