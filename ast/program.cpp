#include "program.h"

using namespace std;
using namespace gw_logic;

ast::Statement::Statement(Expression* expression):
        expression(expression) { }

void ast::Statement::print(ostream& stream) const {
    expression->print(stream);
}

ast::Line::Line(int lineNumber, vector<ast::Statement*>* statementList, char* comment):
        lineNumber(lineNumber),
        statementList(statementList),
        comment(comment) { }

void ast::Line::print(ostream& stream) const {
    stream << "set_line(" << lineNumber << ");";

    if (comment[0] != '\0') {
        stream << " //" << comment;
    }

    stream << endl;

    for (auto statement : *statementList) {
        statement->print(stream);
        stream << ';' << endl;
    }
}
