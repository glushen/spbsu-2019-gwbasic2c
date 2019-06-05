#pragma once
#include <string>
#include <vector>
#include <set>
#include <ostream>
#include <memory>
#include <gw_logic.h>
#include <cmake-build-debug/gw_logic.h>

namespace ast {
    std::string to_string(gw_logic::Type type);

    class ProgramInfo {
    public:
        std::set<std::string> variableDefinitions;
        std::set<const gw_logic::CoreFile*> coreFiles;
        std::set<const gw_logic::LogicFile*> logicFiles;
    };

    class Node {
    public:
        virtual void provideInfo(ProgramInfo& programInfo) const = 0;
        virtual void print(std::ostream& stream) const = 0;
    protected:
        virtual ~Node();
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