#pragma once
#include <utility>
#include <memory>
#include <vector>
#include <ast/expression.h>
#include <ast/program.h>

namespace ph {
    template<typename T>
    T unwrap(T* ptr) {
        T value = std::move(*ptr);
        delete ptr;
        return value;
    }

    std::vector<std::unique_ptr<ast::Expression>>* newExpressionVector();
    std::vector<std::unique_ptr<ast::Expression>>* newExpressionVector(ast::Expression* expression);

    ast::FunctionExpression* asFunction(const std::string& name, std::vector<ast::Expression*> argumentList);
    ast::FunctionExpression* asFunction2(const std::string& name, std::vector<std::unique_ptr<ast::Expression>> argumentList);

    std::unique_ptr<ast::Expression> intoUniquePtr(ast::Expression* expression);

    ast::ForExpression* newForExpression(ast::VariableExpression* variable, ast::Expression* initValue, ast::Expression* finalValue, ast::Expression* stepValue, int& utilityVariablesCount);

    void handleResult(std::vector<ast::Line> lines);
}
