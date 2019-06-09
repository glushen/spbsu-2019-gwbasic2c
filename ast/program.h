#pragma once

#include <algorithm>
#include "expression.h"

namespace ast {
    class Line: public Node {
    public:
        int lineNumber;
        std::vector<std::unique_ptr<ast::Expression>> statementList;
        std::string comment;
        Line(int line_number, std::vector<std::unique_ptr<ast::Expression>> statementList, std::string comment);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    void printCoreFile(std::ostream& stream,
                       const gw::core::File* coreFile,
                       std::set<const gw::core::File*>& printedCoreFiles);

    void printLogicFile(std::ostream& stream,
                        const gw::logic::File* logicFile,
                        std::set<const gw::core::File*>& printedCoreFiles,
                        std::set<const gw::logic::File*>& printedLogicFiles);

    void printProgram(std::ostream& stream, std::vector<ast::Line> lines);
}
