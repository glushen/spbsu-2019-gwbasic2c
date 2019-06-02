#include <utility>
#include "ast.h"
#include <cassert>
#include "util.h"

using namespace std;
using namespace gw_logic;

std::string ast::to_string(gw_logic::Type type) {
    switch (type) {
        case INT: return "INT";
        case FLOAT: return "FLOAT";
        case DOUBLE: return "DOUBLE";
        case STRING: return "STRING";
        case INT_REF: return "INT_REF";
        case FLOAT_REF: return "FLOAT_REF";
        case DOUBLE_REF: return "DOUBLE_REF";
        case STRING_REF: return "STRING_REF";
        case VOID: return "VOID";
    }
    assert(false);
}

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

            bool typeIsCastableImplicitly = castableImplicitly(actualType, expectedType);
            logicFileIsCorrect = logicFileIsCorrect && typeIsCastableImplicitly;

            bool typeIsCastable = typeIsCastableImplicitly || castableExplicitly(actualType, expectedType);
            logicFileIsCorrectable = logicFileIsCorrectable && typeIsCastable;
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
            argumentList[i] = castOrThrow(argumentList[i], correctableLogicFile->argumentTypeList[i]);
        }

        return new FunctionExpression(correctableLogicFile, std::move(argumentList));
    }

    throw std::invalid_argument("Function " + name + " with required signature is not found");
}

ast::CastedExpression::CastedExpression(const ast::Expression* expression, gw_logic::Type type):
    Expression(type),
    expression(expression) { }

void ast::CastedExpression::print(std::ostream& stream) const {
    expression->print(stream);
}

bool ast::castableImplicitly(gw_logic::Type sourceType, gw_logic::Type targetType) {
    if (sourceType == targetType) return true;

    switch (targetType) {
        case DOUBLE:
            switch (sourceType) {
                case DOUBLE_REF:
                case FLOAT:
                case FLOAT_REF:
                case INT:
                case INT_REF:
                    return true;
                default:
                    return false;
            }
        case FLOAT:
            switch (sourceType) {
                case FLOAT_REF:
                case INT:
                case INT_REF:
                    return true;
                default:
                    return false;
            }
        case INT:
            return sourceType == INT_REF;
        case STRING:
            return sourceType == STRING_REF;
        default:
            return false;
    }
}

bool ast::castableExplicitly(gw_logic::Type sourceType, gw_logic::Type targetType) {
    switch (targetType) {
        case INT:
            switch (sourceType) {
                case FLOAT:
                case FLOAT_REF:
                case DOUBLE:
                case DOUBLE_REF:
                    return true;
                default:
                    return false;
            }
        case FLOAT:
            switch (sourceType) {
                case DOUBLE:
                case DOUBLE_REF:
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}

ast::Expression* ast::castOrThrow(const ast::Expression* expression, gw_logic::Type targetType) {
    if (castableImplicitly(expression->type, targetType)) {
        return new CastedExpression(expression, targetType);
    } else if (castableExplicitly(expression->type, targetType)) {
        switch (targetType) {
            case INT:
                return retrieveFunctionExpression("cint", {expression});
            case FLOAT:
                return retrieveFunctionExpression("csng", {expression});
            default:
                assert(false);
        }
    } else {
        throw std::invalid_argument("Cannot cast " + to_string(expression->type) + " to " + to_string(targetType));
    }
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
