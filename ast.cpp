#include <string>
#include <iostream>
#include <vector>

struct Statement {
    char* statement;

    explicit Statement(char* statement): statement(statement) { }

    void print() {
        std::cout << statement;
    }
};

struct Line {
    int line_number;
    std::vector<Statement*>* statement_list;
    char* comment;

    Line(int line_number, std::vector<Statement*>* statement_list, char* comment):
        line_number(line_number), statement_list(statement_list), comment(comment) { }

    void print() {
        std::cout << line_number << " ";
        bool firstPrinted = false;
        for (auto statement : *statement_list) {
            if (firstPrinted) {
                std::cout << " :";
            } else {
                firstPrinted = true;
            }
            statement->print();
        }
        std::cout << " '" << comment;
    }
};

class NumExp {
protected:
    virtual ~NumExp() = default;
};
class IntExp: public NumExp { };
class FloatExp: public NumExp { };
class DoubleExp: public NumExp { };

class IntCaster: public IntExp {
    NumExp* exp;
    explicit IntCaster(NumExp* exp): exp(exp) { }
public:
    static IntExp* cast(NumExp* exp) {
        auto casted = dynamic_cast<IntExp*>(exp);
        return casted != nullptr ? casted : new IntCaster(exp);
    }
};
class FloatCaster: public FloatExp {
    NumExp* exp;
    explicit FloatCaster(NumExp* exp): exp(exp) { }
public:
    static FloatExp* cast(NumExp* exp) {
        auto casted = dynamic_cast<FloatExp*>(exp);
        return casted != nullptr ? casted : new FloatCaster(exp);
    }
};
class DoubleCaster: public DoubleExp {
    NumExp* exp;
    explicit DoubleCaster(NumExp* exp): exp(exp) { }
public:
    static DoubleExp* cast(NumExp* exp) {
        auto casted = dynamic_cast<DoubleExp*>(exp);
        return casted != nullptr ? casted : new DoubleCaster(exp);
    }
};

class IntConst: public IntExp {
    int value;
public:
    explicit IntConst(int value): value(value) { }
};
class FloatConst: public FloatExp {
    float value;
public:
    explicit FloatConst(float value): value(value) { }
};
class DoubleConst: public DoubleExp {
    double value;
public:
    explicit DoubleConst(double value): value(value) { }
};

class UnaryNumOp: public NumExp {
    const char* name;
    NumExp* operand;
public:
    UnaryNumOp(const char* name, NumExp* operand):
        name(name), operand(operand) { }
};

class BinaryNumOp: public NumExp {
    const char* name;
    NumExp* left_operand;
    NumExp* right_operand;
public:
    BinaryNumOp(const char* name, NumExp* left_operand, NumExp* right_operand):
        name(name), left_operand(left_operand), right_operand(right_operand) { }
};

class StringExp {
protected:
    virtual ~StringExp() = default;
};

class StringConst: public StringExp {
    char* value;
public:
    explicit StringConst(char* value): value(value) { }
};