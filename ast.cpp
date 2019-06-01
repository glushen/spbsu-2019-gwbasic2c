#include <utility>
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
        ConstExpression(FLOAT, util::to_string("%.7g", value)) { }

ast::DoubleConstExpression::DoubleConstExpression(double value):
        ConstExpression(DOUBLE, util::to_string("%.16g", value)) { }

ast::StringConstExpression::StringConstExpression(const std::string& value):
        ConstExpression(STRING, '"' + util::escape(value) + '"') { }

ast::VariableExpression::VariableExpression(std::string name, gw_logic::Type type):
        Expression(type),
        name(std::move(name)) {
    assert(type == INT_REF || type == FLOAT_REF || type == DOUBLE_REF || type == STRING_REF);
}

void ast::VariableExpression::print(ostream& stream) const {
    stream << '&' << name;
}

ast::FunctionExpression::FunctionExpression(const LogicFile* logicFile, vector<const Expression*> argumentList):
        Expression(logicFile->returnType),
        logicFile(logicFile),
        argumentList(std::move(argumentList)) { }

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

ast::FunctionExpression* ast::retrieveFunctionExpression(const string& name, vector<const Expression*> argumentList) {
    if (LOGIC_FILES_BY_GW_FUNCTION_NAME.count(name) == 0) {
        throw std::invalid_argument("Function " + name + " is not found");
    }

    auto& logicFiles = LOGIC_FILES_BY_GW_FUNCTION_NAME.at(name);
    const LogicFile* correctableLogicFile = nullptr;

    for (auto logicFile: logicFiles) {
        if (logicFile->argumentTypeList.size() != argumentList.size()) {
            continue;
        }

        bool logicFileIsCorrect = true;
        bool logicFileIsCorrectable = true;

        for (int i = 0; i < argumentList.size(); i++) {
            Type actualType = argumentList[i]->type;
            Type expectedType = logicFile->argumentTypeList[i];

            bool typeIsCorrect = actualType == expectedType
                                 || (actualType == INT && (expectedType == FLOAT || expectedType == DOUBLE))
                                 || (actualType == FLOAT && expectedType == DOUBLE);
            logicFileIsCorrect = logicFileIsCorrect && typeIsCorrect;

            bool canBeCasted = (expectedType == INT && (actualType == FLOAT || actualType == DOUBLE))
                               || (expectedType == FLOAT && actualType == DOUBLE);
            logicFileIsCorrectable = logicFileIsCorrectable && (typeIsCorrect || canBeCasted);
        }

        if (logicFileIsCorrect) {
            return new FunctionExpression(logicFile, std::move(argumentList));
        }

        if (logicFileIsCorrectable) {
            correctableLogicFile = logicFile;
        }
    }

    if (correctableLogicFile != nullptr) {
        for (int i = 0; i < argumentList.size(); i++) {
            Type actualType = argumentList[i]->type;
            Type expectedType = correctableLogicFile->argumentTypeList[i];

            if (expectedType == INT && (actualType == FLOAT || actualType == DOUBLE)) {
                argumentList[i] = ast::retrieveFunctionExpression("cint", {argumentList[i]});
            } else if (expectedType == FLOAT && actualType == DOUBLE) {
                argumentList[i] = ast::retrieveFunctionExpression("csng", {argumentList[i]});
            }
        }

        return new FunctionExpression(correctableLogicFile, std::move(argumentList));
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
