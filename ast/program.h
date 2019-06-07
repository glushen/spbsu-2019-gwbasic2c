#pragma once

#include <algorithm>
#include "expression.h"

namespace ast {
    class Line: public Node {
    public:
        int lineNumber;
        std::vector<std::unique_ptr<ast::Expression>> statementList;
        char* comment;
        Line(int line_number, std::vector<std::unique_ptr<ast::Expression>> statementList, char* comment);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    void printCoreFile(std::ostream& stream,
                       const gw_logic::CoreFile* coreFile,
                       std::set<const gw_logic::CoreFile*>& printedCoreFiles);

    void printLogicFile(std::ostream& stream,
                        const gw_logic::LogicFile* logicFile,
                        std::set<const gw_logic::CoreFile*>& printedCoreFiles,
                        std::set<const gw_logic::LogicFile*>& printedLogicFiles);

    void printProgram(std::ostream& stream, std::vector<ast::Line*>* lines);
}
