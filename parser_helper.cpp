#include <utility>
#include <iostream>
#include "parser_helper.h"

std::vector<std::unique_ptr<ast::Expression>>* ph::newExpressionVector() {
    return new std::vector<std::unique_ptr<ast::Expression>>;
}

std::vector<std::unique_ptr<ast::Expression>>* ph::newExpressionVector(ast::Expression* expression) {
    auto result = new std::vector<std::unique_ptr<ast::Expression>>;
    result->emplace_back(expression);
    return result;
}

ast::FunctionExpression* ph::asFunction(const std::string& name, std::vector<ast::Expression*> argumentList) {
    std::vector<std::unique_ptr<ast::Expression>> convertedArgumentList;
    convertedArgumentList.reserve(argumentList.size());
    for (auto& argument : argumentList) {
        convertedArgumentList.emplace_back(argument);
    }
    return ph::asFunction2(name, std::move(convertedArgumentList));
}

ast::FunctionExpression* ph::asFunction2(const std::string& name, std::vector<std::unique_ptr<ast::Expression>> argumentList) {
    return ast::asFunction(name, std::move(argumentList)).release();
}

std::unique_ptr<ast::Expression> ph::intoUniquePtr(ast::Expression* expression) {
    return std::unique_ptr<ast::Expression>(expression);
}

void ph::handleResult(std::vector<ast::Line> lines) {
    ast::printProgram(std::cout, std::move(lines));
}

ast::ForExpression* ph::newForExpression(ast::VariableExpression* mainVariablePointer,
                                         ast::Expression* initValuePointer,
                                         ast::Expression* finalValuePointer,
                                         ast::Expression* stepValuePointer,
                                         int& utilityVariablesCount) {
    auto mainVariable = unwrap(mainVariablePointer);
    auto initialization1 = asFunction("let", {new ast::VariableExpression(mainVariable), initValuePointer});

    auto finalVariable = ast::VariableExpression(std::to_string(utilityVariablesCount++), mainVariable.type);
    auto initialization2 = asFunction("let", {new ast::VariableExpression(finalVariable), finalValuePointer});

    auto stepVariable = ast::VariableExpression(std::to_string(utilityVariablesCount++), mainVariable.type);
    auto initialization3 = asFunction("let", {new ast::VariableExpression(stepVariable), stepValuePointer});

    auto mainVariableMulSignOfStep = asFunction("mul", {new ast::VariableExpression(mainVariable), asFunction("sgn", {new ast::VariableExpression(stepVariable)})});
    auto finalVariableMulSignOfStep = asFunction("mul", {new ast::VariableExpression(finalVariable), asFunction("sgn", {new ast::VariableExpression(stepVariable)})});
    auto condition = asFunction("leq", {mainVariableMulSignOfStep, finalVariableMulSignOfStep});

    auto mainVariablePlusStep = asFunction("sum", {new ast::VariableExpression(mainVariable), new ast::VariableExpression(stepVariable)});
    auto updating = asFunction("let", {new ast::VariableExpression(mainVariable), mainVariablePlusStep});

    return new ast::ForExpression(std::move(mainVariable),
                                  intoUniquePtr(initialization1),
                                  intoUniquePtr(initialization2),
                                  intoUniquePtr(initialization3),
                                  intoUniquePtr(condition),
                                  intoUniquePtr(updating));
}
