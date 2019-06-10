#pragma once
#include "node.h"

namespace ast {
    class Expression: public Node {
    public:
        const gw::Type type;
        explicit Expression(gw::Type type);
    };

    class ConstExpression: public Expression {
    public:
        ConstExpression(gw::Type type, std::string&& valueToPrint);
        void provideInfo(ProgramInfo& programInfo) const override;
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
        VariableExpression(std::string name, gw::Type type);
        std::string getPrintableType() const;
        std::string getPrintableName() const;
        std::string getPrintableDefaultValue() const;
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class VectorDimExpression: public Expression {
    public:
        const VariableExpression variable;
        const std::vector<std::unique_ptr<Expression>> new_sizes;
        VectorDimExpression(VariableExpression variable, std::vector<std::unique_ptr<Expression>> new_sizes);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class VectorGetElementExpression: public Expression {
    public:
        const VariableExpression variable;
        const std::vector<std::unique_ptr<Expression>> indexes;
        VectorGetElementExpression(VariableExpression variable, std::vector<std::unique_ptr<Expression>> indexes);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class FunctionExpression: public Expression {
    public:
        const gw::logic::File* logicFile;
        const std::vector<std::unique_ptr<Expression>> argumentList;
        FunctionExpression(const gw::logic::File* logicFile, std::vector<std::unique_ptr<Expression>> argumentList);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    std::unique_ptr<FunctionExpression> asFunction(const std::string& name, std::vector<std::unique_ptr<Expression>> argumentList);

    class CastedExpression: public Expression {
    public:
        const std::unique_ptr<Expression> expression;
        CastedExpression(std::unique_ptr<Expression> expression, gw::Type type);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    bool castableImplicitly(gw::Type sourceType, gw::Type targetType);
    bool castableExplicitly(gw::Type sourceType, gw::Type targetType);
    std::unique_ptr<Expression> castOrThrow(std::unique_ptr<Expression> expression, gw::Type targetType);
    std::vector<std::unique_ptr<Expression>> castOrThrow(std::vector<std::unique_ptr<Expression>> expressions, gw::Type targetType);
    std::unique_ptr<Expression> convertToString(std::unique_ptr<Expression> expression);

    class PrintExpression: public Expression {
    public:
        bool printNewLine = true;
        PrintExpression();
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
        void addExpression(std::unique_ptr<Expression> expression);
    private:
        std::vector<std::unique_ptr<Expression>> expressions;
        const StringConstExpression newLineExpression;
    };

    class InputExpression: public Expression {
    public:
        const std::unique_ptr<Expression> prompt;
        const std::vector<std::unique_ptr<Expression>> expressions;
        InputExpression(std::unique_ptr<Expression>, std::vector<std::unique_ptr<Expression>> expressions);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class GotoExpression: public Expression {
    public:
        const int lineNumber;
        explicit GotoExpression(int lineNumber);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class OnGotoExpression: public Expression {
    public:
        const std::unique_ptr<Expression> expression;
        const std::vector<int> lineNumbers;
        OnGotoExpression(std::unique_ptr<Expression> expression, std::vector<int> lineNumbers);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class IfExpression: public Expression {
    public:
        const std::unique_ptr<Expression> condition;
        const std::vector<std::unique_ptr<Expression>> thanStatements;
        const std::vector<std::unique_ptr<Expression>> elseStatements;
        IfExpression(std::unique_ptr<Expression> condition, std::vector<std::unique_ptr<Expression>> thanStatements, std::vector<std::unique_ptr<Expression>> elseStatements);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class WhileExpression: public Expression {
    public:
        const std::unique_ptr<Expression> condition;
        explicit WhileExpression(std::unique_ptr<Expression> condition);
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };

    class WendExpression: public Expression {
    public:
        WendExpression();
        void provideInfo(ProgramInfo& programInfo) const override;
        void print(std::ostream& stream) const override;
    };
}
