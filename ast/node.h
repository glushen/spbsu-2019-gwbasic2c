#pragma once
#include <string>
#include <vector>
#include <set>
#include <ostream>
#include <memory>
#include <gw.h>

namespace ast {
    std::string to_string(gw::Type type);
    bool isReference(gw::Type type);

    class ProgramInfo {
    public:
        std::set<std::string> variableDefinitions;
        std::set<const gw::core::File*> coreFiles;
        std::set<const gw::logic::File*> logicFiles;
        std::set<int> requiredLineNumbers;
        int openedWhileLoopsCount = 0;
        std::vector<std::string> forLoopsVariables;
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