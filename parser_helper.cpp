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
