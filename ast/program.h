#pragma once
#include "expression.h"

namespace ast {
    class Statement: public Printable {
    public:
        Expression* expression;
        explicit Statement(Expression* expression);
        void print(std::ostream& stream) const override;
    };

    class Line: public Printable {
    public:
        int lineNumber;
        std::vector<Statement*>* statementList;
        char* comment;
        Line(int line_number, std::vector<Statement*>* statementList, char* comment);
        void print(std::ostream& stream) const override;
    };
}
