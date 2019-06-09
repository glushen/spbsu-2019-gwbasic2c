#include <utility>
#include <memory>
#include <cassert>
#include "node.h"
#include "expression.h"
#include "util.h"

using namespace std;
using namespace gw_logic;

ast::Expression::Expression(Type type):
        type(type) { }

ast::ConstExpression::ConstExpression(gw_logic::Type type, std::string&& valueToPrint):
        Expression(type),
        valueToPrint(valueToPrint) { }

void ast::ConstExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    // nothing to provide
}

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

std::string ast::VariableExpression::getPrintableType() const {
    switch (type) {
        case INT_REF: return "gw_int";
        case FLOAT_REF: return "float";
        case DOUBLE_REF: return "double";
        case STRING_REF: return "std::string";
        default: assert(false);
    }
}

std::string ast::VariableExpression::getPrintableName() const {
    switch (type) {
        case INT_REF: return "_" + name + "_i";
        case FLOAT_REF: return "_" + name + "_f";
        case DOUBLE_REF: return "_" + name + "_d";
        case STRING_REF: return "_" + name + "_s";
        default: assert(false);
    }
}

std::string ast::VariableExpression::getPrintableDefaultValue() const {
    return type == STRING_REF ? "\"\"" : "0";
}

void ast::VariableExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    programInfo.variableDefinitions.insert(getPrintableType() + " " + getPrintableName() + " = " + getPrintableDefaultValue());
}

void ast::VariableExpression::print(ostream& stream) const {
    stream << getPrintableName();
}

ast::VectorDimExpression::VectorDimExpression(VariableExpression variable, std::vector<std::unique_ptr<Expression>> new_sizes):
        Expression(VOID),
        variable(std::move(variable)),
        new_sizes(castOrThrow(std::move(new_sizes), INT)) { }

void ast::VectorDimExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    programInfo.variableDefinitions.insert("std::vector<" + variable.getPrintableType() + "> " + variable.getPrintableName() + "v = { }");
    programInfo.variableDefinitions.insert("std::vector<gw_int> " + variable.getPrintableName() + "i = { }");
    programInfo.coreFiles.insert(gw_logic::core_vector);
    for (auto& child : new_sizes) {
        child->provideInfo(programInfo);
    }
}

void ast::VectorDimExpression::print(std::ostream& stream) const {
    stream << "dim(" << variable.getPrintableName() << "v," << variable.getPrintableName() << "i,{";
    joinAndPrint(stream, new_sizes);
    stream << "})";
}

ast::VectorGetElementExpression::VectorGetElementExpression(VariableExpression variable, std::vector<std::unique_ptr<Expression>> indexes):
        Expression(variable.type),
        variable(std::move(variable)),
        indexes(castOrThrow(std::move(indexes), INT)) { }

void ast::VectorGetElementExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    programInfo.variableDefinitions.insert("std::vector<" + variable.getPrintableType() + "> " + variable.getPrintableName() + "v = { }");
    programInfo.variableDefinitions.insert("std::vector<gw_int> " + variable.getPrintableName() + "i = { }");
    programInfo.coreFiles.insert(gw_logic::core_vector);
    for (auto& child : indexes) {
        child->provideInfo(programInfo);
    }
}

void ast::VectorGetElementExpression::print(std::ostream& stream) const {
    stream << "get(" << variable.getPrintableName() << "v," << variable.getPrintableName() << "i,{";
    joinAndPrint(stream, indexes);
    stream << "})";
}

ast::FunctionExpression::FunctionExpression(const LogicFile* logicFile, std::vector<std::unique_ptr<Expression>> argumentList):
        Expression(logicFile->returnType),
        logicFile(logicFile),
        argumentList(std::move(argumentList)) { }

void ast::FunctionExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    programInfo.logicFiles.insert(logicFile);
    for (auto& child : argumentList) {
        child->provideInfo(programInfo);
    }
}

void ast::FunctionExpression::print(ostream& stream) const {
    stream << logicFile->name << '(';
    joinAndPrint(stream, argumentList);
    stream << ')';
}

std::unique_ptr<ast::FunctionExpression> ast::retrieveFunctionExpression(const string& name, vector<unique_ptr<Expression>> argumentList) {
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
            return std::make_unique<FunctionExpression>(logicFile, std::move(argumentList));
        }

        if (logicFileIsCorrectable) {
            correctableLogicFile = logicFile;
        }
    }

    if (correctableLogicFile != nullptr) {
        for (int i = 0; i < argumentList.size(); i++) {
            argumentList[i] = castOrThrow(std::move(argumentList[i]), correctableLogicFile->argumentTypeList[i]);
        }

        return std::make_unique<ast::FunctionExpression>(correctableLogicFile, std::move(argumentList));
    }

    throw std::invalid_argument("Function " + name + " with required signature is not found");
}

ast::CastedExpression::CastedExpression(std::unique_ptr<Expression> expression, gw_logic::Type type):
        Expression(type),
        expression(std::move(expression)) { }

void ast::CastedExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    expression->provideInfo(programInfo);
}

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

std::unique_ptr<ast::Expression> ast::castOrThrow(std::unique_ptr<ast::Expression> expression, gw_logic::Type targetType) {
    if (castableImplicitly(expression->type, targetType)) {
        return std::make_unique<CastedExpression>(std::move(expression), targetType);
    } else if (castableExplicitly(expression->type, targetType)) {
        string functionName;
        switch (targetType) {
            case INT:
                functionName = "cint";
                break;
            case FLOAT:
                functionName = "csng";
                break;
            default:
                assert(false);
                break;
        }
        vector<unique_ptr<Expression>> argumentList;
        argumentList.push_back(std::move(expression));
        return std::move(retrieveFunctionExpression(functionName, std::move(argumentList)));
    } else {
        throw std::invalid_argument("Cannot cast " + to_string(expression->type) + " to " + to_string(targetType));
    }
}

std::vector<std::unique_ptr<ast::Expression>> ast::castOrThrow(std::vector<std::unique_ptr<Expression>> expressions, gw_logic::Type targetType) {
    for (auto& expression : expressions) {
        expression = castOrThrow(std::move(expression), targetType);
    }
    return expressions;
}

unique_ptr<ast::Expression> ast::convertToString(unique_ptr<ast::Expression> expression) {
    if (expression->type == STRING || expression->type == STRING_REF) {
        return std::make_unique<CastedExpression>(std::move(expression), STRING);
    } else {
        vector<unique_ptr<Expression>> arguments;
        arguments.push_back(move(expression));
        return ast::retrieveFunctionExpression("str$", move(arguments));
    }
}

ast::PrintExpression::PrintExpression():
        Expression(VOID),
        newLineExpression("\n") { }

void ast::PrintExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    for (auto& expression : expressions) {
        expression->provideInfo(programInfo);
    }
    programInfo.coreFiles.insert(gw_logic::core_print);
    if (printNewLine) {
        newLineExpression.provideInfo(programInfo);
    }
}

void ast::PrintExpression::print(std::ostream& stream) const {
    stream << "print({";
    joinAndPrint(stream, expressions);

    if (printNewLine) {
        if (!expressions.empty()) {
            stream << ',';
        }
        newLineExpression.print(stream);
    }

    stream << "})";
}

void ast::PrintExpression::addExpression(unique_ptr<ast::Expression> expression) {
    expressions.push_back(convertToString(move(expression)));
}

ast::InputExpression::InputExpression(std::unique_ptr<Expression> prompt, vector<unique_ptr<ast::Expression>> expressions):
        Expression(VOID),
        prompt(std::move(prompt)),
        expressions(std::move(expressions)) {
    for (auto& expression : expressions) {
        if (!isReference(expression->type)) {
            throw std::invalid_argument("Expected reference, found " + to_string(expression->type));
        }
    }
}

void ast::InputExpression::provideInfo(ast::ProgramInfo& programInfo) const {
    prompt->provideInfo(programInfo);
    for (auto& expression : expressions) {
        expression->provideInfo(programInfo);
    }
    programInfo.coreFiles.insert(gw_logic::core_input);
}

void ast::InputExpression::print(std::ostream& stream) const {
    stream << "input(";
    prompt->print(stream);
    stream << ",{";
    joinAndPrint(stream, expressions);
    stream << "})";
}
