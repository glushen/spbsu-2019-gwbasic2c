%code requires {
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <memory>
#include <ast/program.h>

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s, ...);
void handleResult(std::vector<ast::Line> lines);

template<typename T> T move_ptr(T* ptr) {
    T value = std::move(*ptr);
    delete ptr;
    return value;
}
template<typename T> std::vector<std::unique_ptr<T>> move_vector(std::vector<T*> vector) {
    std::vector<std::unique_ptr<T>> result;
    result.reserve(vector.size());

    for (auto& item : vector) {
        result.push_back(std::unique_ptr<T>(item));
    }

    return result;
}
}

%union {
    int line_number;
    std::string* comment;
    std::vector<ast::Line>* lines;
    ast::Line* line;
    std::vector<std::unique_ptr<ast::Expression>>* expressions;
    std::string* name;
    ast::Expression* exp;
    ast::VariableExpression* variable;
}

%token <exp> CONST
%token <variable> VARIABLE
%token <name> GW_FN_NAME_UNSUPPORTED GW_CMD_NAME_UNSUPPORTED GW_STM_NAME_UNSUPPORTED
%token <name> GW_FN_NAME GW_CMD_NAME GW_STM_NAME
%token <name> FN_VAR UNSUPPORTED_VAR
%token LET_KEYWORD DIM_KEYWORD TRON_KEYWORD TROFF_KEYWORD
%token MOD_OPERATOR
%token EQUAL_OPERATOR UNEQUAL_OPERATOR LESS_OPERATOR GREATER_OPERATOR LESS_EQUAL_OPERATOR GREATER_EQUAL_OPERATOR
%token NOT_OPERATOR AND_OPERATOR OR_OPERATOR XOR_OPERATOR EQV_OPERATOR IMP_OPERATOR
%token <line_number> LINE_NUMBER
%token <comment> COMMENT
%token END_OF_FILE

%type <comment> OPTIONAL_COMMENT
%type <lines> PROGRAM LINE_LIST
%type <line> LINE
%type <expressions> STATEMENT_LIST NOT_EMPTY_STATEMENT_LIST
%type <exp> EXP
%type <expressions> EXP_LIST NOT_EMPTY_EXP_LIST
%type <exp> LVALUE

%left IMP_OPERATOR
%left EQV_OPERATOR
%left XOR_OPERATOR
%left OR_OPERATOR
%left AND_OPERATOR
%nonassoc NOT_OPERATOR
%left EQUAL_OPERATOR UNEQUAL_OPERATOR LESS_OPERATOR GREATER_OPERATOR LESS_EQUAL_OPERATOR GREATER_EQUAL_OPERATOR
%left '-'
%left '+'
%left MOD_OPERATOR
%left '\\'
%left '/'
%left '*'
%nonassoc UNARY_MINUS
%left '^'

%%

PROGRAM:
    LINE_LIST END_OF_FILE { $$ = $1; handleResult(move_ptr($$)); YYACCEPT; }

LINE_LIST:
    LINE                { $$ = new std::vector<ast::Line>(); if ($1 != nullptr) $$->push_back(move_ptr($1)); }
|   LINE_LIST '\n' LINE { $$ = $1; if ($3 != nullptr) $$->push_back(move_ptr($3)); }

LINE:
    %empty                                      { $$ = nullptr; }
|   LINE_NUMBER STATEMENT_LIST OPTIONAL_COMMENT { $$ = new ast::Line($1, move_ptr($2), move_ptr($3)); }

STATEMENT_LIST:
    %empty                   { $$ = new std::vector<std::unique_ptr<ast::Expression>>(); }
|   NOT_EMPTY_STATEMENT_LIST { $$ = $1; }

NOT_EMPTY_STATEMENT_LIST:
    EXP                               { $$ = new std::vector<std::unique_ptr<ast::Expression>>(); $$->push_back(std::unique_ptr<ast::Expression>($1)); }
|   NOT_EMPTY_STATEMENT_LIST ':' EXP  { $$ = $1; $$->push_back(std::unique_ptr<ast::Expression>($3)); }

OPTIONAL_COMMENT:
    %empty           { $$ = new std::string(); }
|   COMMENT          { $$ = $1; }

OPTIONAL_LET_KEYWORD:
    %empty
|   LET_KEYWORD

EXP_LIST:
    %empty              { $$ = new std::vector<std::unique_ptr<ast::Expression>>(); }
|   NOT_EMPTY_EXP_LIST  { $$ = $1; }

NOT_EMPTY_EXP_LIST:
    EXP                         { $$ = new std::vector<std::unique_ptr<ast::Expression>>(); $$->push_back(std::unique_ptr<ast::Expression>($1)); }
|   NOT_EMPTY_EXP_LIST ',' EXP  { $$ = $1; $$->push_back(std::unique_ptr<ast::Expression>($3)); }

LVALUE:
    VARIABLE                   { $$ = $1; }
|   VARIABLE '(' EXP_LIST ')'  { $$ = new ast::VectorGetElementExpression(move_ptr($1), move_ptr($3)); }

EXP:
    CONST                      { $$ = $1; }
|   '(' EXP ')'                { $$ = $2; }
|   EXP '^' EXP                { $$ = ast::retrieveFunctionExpression("pow", move_vector<ast::Expression>({$1, $3})).release(); }
|   '-' EXP %prec UNARY_MINUS  { $$ = ast::retrieveFunctionExpression("neg", move_vector<ast::Expression>({$2})).release(); }
|   EXP '*' EXP                { $$ = ast::retrieveFunctionExpression("mul", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '/' EXP                { $$ = ast::retrieveFunctionExpression("fdiv", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '\\' EXP               { $$ = ast::retrieveFunctionExpression("idiv", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP MOD_OPERATOR EXP       { $$ = ast::retrieveFunctionExpression("mod", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '+' EXP                { $$ = ast::retrieveFunctionExpression("sum", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '-' EXP                { $$ = ast::retrieveFunctionExpression("sub", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP EQUAL_OPERATOR EXP          { $$ = ast::retrieveFunctionExpression("equal", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP UNEQUAL_OPERATOR EXP        { $$ = ast::retrieveFunctionExpression("unequal", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP LESS_OPERATOR EXP           { $$ = ast::retrieveFunctionExpression("less", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP GREATER_OPERATOR EXP        { $$ = ast::retrieveFunctionExpression("greater", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP LESS_EQUAL_OPERATOR EXP     { $$ = ast::retrieveFunctionExpression("geq", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP GREATER_EQUAL_OPERATOR EXP  { $$ = ast::retrieveFunctionExpression("leq", move_vector<ast::Expression>({$1, $3})).release(); }
|   NOT_OPERATOR EXP                { $$ = ast::retrieveFunctionExpression("not", move_vector<ast::Expression>({$2})).release(); }
|   EXP AND_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("and", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP OR_OPERATOR EXP             { $$ = ast::retrieveFunctionExpression("or", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP XOR_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("xor", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP EQV_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("eqv", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP IMP_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("imp", move_vector<ast::Expression>({$1, $3})).release(); }
|   GW_FN_NAME '(' EXP_LIST ')'     { $$ = ast::retrieveFunctionExpression(move_ptr($1), move_ptr($3)).release(); }
|   OPTIONAL_LET_KEYWORD LVALUE EQUAL_OPERATOR EXP  { $$ = ast::retrieveFunctionExpression("let", move_vector<ast::Expression>({$2, $4})).release(); }
|   DIM_KEYWORD VARIABLE '(' EXP_LIST ')'  { $$ = new ast::VectorDimExpression(move_ptr($2), move_ptr($4)); }
|   TRON_KEYWORD                    { $$ = ast::retrieveFunctionExpression("tron", {}).release(); }
|   TROFF_KEYWORD                   { $$ = ast::retrieveFunctionExpression("troff", {}).release(); }
