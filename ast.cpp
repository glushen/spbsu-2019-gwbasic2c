#include "ast.h"
#include <cassert>
#include "util.h"

using namespace std;
using namespace gw_logic;

ast::Printable::~Printable() = default;

ast::Expression::Expression(Type type):
        type(type) { }

ast::ConstExpression::ConstExpression(gw_logic::Type type, std::string&& valueToPrint):
        Expression(type),
        valueToPrint(valueToPrint) { }

void ast::ConstExpression::print(ostream& stream) const {
    stream << valueToPrint;
}

ast::IntConstExpression::IntConstExpression(short value):
        ConstExpression(INT, util::to_string("%d", value)) { }

ast::FloatConstExpression::FloatConstExpression(float value):
        ConstExpression(FLOAT, util::to_string("%.9f", value)) { }

ast::DoubleConstExpression::DoubleConstExpression(double value):
        ConstExpression(DOUBLE, util::to_string("%.17f", value)) { }

ast::StringConstExpression::StringConstExpression(const std::string& value):
        ConstExpression(STRING, '"' + util::escape(value) + '"') { }

ast::VariableExpression::VariableExpression(string&& name, Type type):
        Expression(type),
        name(name) {
    assert(type == INT_PTR || type == FLOAT_PTR || type == DOUBLE_PTR || type == STRING_PTR);
}

void ast::VariableExpression::print(ostream& stream) const {
    stream << name; // TODO доделать
}

ast::FunctionExpression::FunctionExpression(const LogicFile* logicFile, vector<const Expression*>&& argumentList):
        Expression(logicFile->returnType),
        logicFile(logicFile),
        argumentList(argumentList) { }

void ast::FunctionExpression::print(ostream& stream) const {
    stream << logicFile->name << '(';
    bool first = true;
    for (auto& argument : argumentList) {
        if (first) {
            first = false;
        } else {
            stream << ',';
        }
        argument->print(stream);
    }
    stream << ')';
}

ast::FunctionExpression* ast::retrieveFunctionExpression(const string& name, vector<const Expression*>&& argumentList) {
    if (LOGIC_FILES_BY_GW_FUNCTION_NAME.count(name) == 0) {
        throw std::invalid_argument("Function " + name + " is not found");
    }

    for (auto logicFile: LOGIC_FILES_BY_GW_FUNCTION_NAME.at(name)) {
        if (logicFile->argumentTypeList.size() != argumentList.size()) {
            continue;
        }

        for (int i = 0; i < argumentList.size(); i++) {
            Type actualType = argumentList[i]->type;
            Type expectedType = logicFile->argumentTypeList[i];

            bool typeIsCorrect = actualType == expectedType
                                 || (actualType == INT && (expectedType == FLOAT || expectedType == DOUBLE))
                                 || (actualType == FLOAT && expectedType == DOUBLE);

            if (typeIsCorrect) {
                return new FunctionExpression(logicFile, forward<vector<const Expression*>>(argumentList));
            }
        }
    }

    throw std::invalid_argument("Function " + name + " with required signature is not found");
}

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
    stream << lineNumber << " ";
    bool firstPrinted = false;
    for (auto statement : *statementList) {
        if (firstPrinted) {
            stream << " :";
        } else {
            firstPrinted = true;
        }
        statement->print(stream);
    }
    stream << " '" << comment;
}
