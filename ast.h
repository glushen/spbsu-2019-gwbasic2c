#pragma once
#include <string>
#include <vector>
#include <ostream>
#include <memory>
#include <gw_logic.h>
#include <cmake-build-debug/gw_logic.h>

namespace ast {
    std::string to_string(gw_logic::Type type);

    class Printable {
    public:
        virtual void print(std::ostream& stream) const = 0;
    protected:
        virtual ~Printable();
    };

    template <typename T>
    void joinAndPrint(std::ostream& stream, const std::vector<T>& values, const std::string& separator = ",");

    class Expression: public Printable {
    public:
        const gw_logic::Type type;
        explicit Expression(gw_logic::Type type);
    };

    class ConstExpression: public Expression {
    public:
        ConstExpression(gw_logic::Type type, std::string&& valueToPrint);
        void print(std::ostream& stream) const override;
    private:
        std::string valueToPrint;
    };

    class IntConstExpression: public ConstExpression {
    public:
        explicit IntConstExpression(short value);
    };

    class FloatConstExpression: public ConstExpression {
    public:
        explicit FloatConstExpression(float value);
    };

    class DoubleConstExpression: public ConstExpression {
    public:
        explicit DoubleConstExpression(double value);
    };

    class StringConstExpression: public ConstExpression {
    public:
        explicit StringConstExpression(const std::string& value);
    };

    class VariableExpression: public Expression {
    public:
        const std::string name;
        VariableExpression(std::string name, gw_logic::Type type);
        void print(std::ostream& stream) const override;
    };

    class VectorDimExpression: public Expression {
    public:
        const VariableExpression* variable;
        const std::vector<std::unique_ptr<Expression>> new_sizes;
        VectorDimExpression(const VariableExpression* variable, std::vector<std::unique_ptr<Expression>> new_sizes);
        void print(std::ostream& stream) const override;
    };

    class VectorGetElementExpression: public Expression {
    public:
        const VariableExpression* variable;
        const std::vector<std::unique_ptr<Expression>> indexes;
        VectorGetElementExpression(const VariableExpression* variable, std::vector<std::unique_ptr<Expression>> indexes);
        void print(std::ostream& stream) const override;
    };

    class FunctionExpression: public Expression {
    public:
        const gw_logic::LogicFile* logicFile;
        const std::vector<const Expression*> argumentList;
        FunctionExpression(const gw_logic::LogicFile* logicFile, std::vector<const Expression*> argumentList);
        void print(std::ostream& stream) const override;
    };

    FunctionExpression* retrieveFunctionExpression(const std::string& name, std::vector<const Expression*> argumentList);

    class CastedExpression: public Expression {
    public:
        const Expression* expression;
        CastedExpression(const Expression* expression, gw_logic::Type type);
        void print(std::ostream& stream) const override;
    };

    bool castableImplicitly(gw_logic::Type sourceType, gw_logic::Type targetType);
    bool castableExplicitly(gw_logic::Type sourceType, gw_logic::Type targetType);
    Expression* castOrThrow(const Expression* expression, gw_logic::Type targetType);
    std::vector<std::unique_ptr<Expression>> castOrThrow(std::vector<std::unique_ptr<Expression>> expression, gw_logic::Type targetType);

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