#pragma once

#include <algorithm>
#include "expression.h"

namespace ast {
    class Statement: public Node {
    public:
        Expression* expression;
        explicit Statement(Expression* expression);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class Line: public Node {
    public:
        int lineNumber;
        std::vector<Statement*>* statementList;
        char* comment;
        Line(int line_number, std::vector<Statement*>* statementList, char* comment);
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
