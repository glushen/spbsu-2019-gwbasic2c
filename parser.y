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
    int lineNumber;
    std::vector<int>* lineNumbers;
    std::string* comment;
    std::vector<ast::Line>* lines;
    ast::Line* line;
    std::vector<std::unique_ptr<ast::Expression>>* expressions;
    std::string* name;
    ast::Expression* exp;
    ast::VariableExpression* variable;
    ast::PrintExpression* printExp;
}

%token <exp> NUM_CONST STRING_CONST
%token <variable> VARIABLE
%token <name> GW_FN_NAME_UNSUPPORTED GW_CMD_NAME_UNSUPPORTED GW_STM_NAME_UNSUPPORTED
%token <name> GW_FN_NAME GW_CMD_NAME GW_STM_NAME
%token <name> FN_VAR UNSUPPORTED_VAR
%token LET_KEYWORD DIM_KEYWORD TRON_KEYWORD TROFF_KEYWORD PRINT_KEYWORD LINE_INPUT_KEYWORD INPUT_KEYWORD ON_KEYWORD GOTO_KEYWORD
%token MOD_OPERATOR
%token EQUAL_OPERATOR UNEQUAL_OPERATOR LESS_OPERATOR GREATER_OPERATOR LESS_EQUAL_OPERATOR GREATER_EQUAL_OPERATOR
%token NOT_OPERATOR AND_OPERATOR OR_OPERATOR XOR_OPERATOR EQV_OPERATOR IMP_OPERATOR
%token <lineNumber> LINE_NUMBER GOTO_STATEMENT
%token <comment> COMMENT
%token END_OF_FILE

%type <lineNumbers> LINE_NUMBER_LIST
%type <comment> OPTIONAL_COMMENT
%type <lines> PROGRAM LINE_LIST
%type <line> LINE
%type <expressions> STATEMENT_LIST NOT_EMPTY_STATEMENT_LIST
%type <exp> EXP STATEMENT
%type <expressions> EXP_LIST NOT_EMPTY_EXP_LIST
%type <exp> LVALUE
%type <expressions> LVALUE_LIST
%type <printExp> PRINT_LIST
%type <exp> OPTIONAL_INPUT_PROMPT_STRING OPTIONAL_LINE_INPUT_PROMPT_STRING

%nonassoc PRINT_KEYWORD
%left IMP_OPERATOR
%left EQV_OPERATOR
%left XOR_OPERATOR
%left OR_OPERATOR
%left AND_OPERATOR
%nonassoc NOT_OPERATOR
%left EQUAL_OPERATOR UNEQUAL_OPERATOR LESS_OPERATOR GREATER_OPERATOR LESS_EQUAL_OPERATOR GREATER_EQUAL_OPERATOR
%nonassoc LOWER_THAN_MINUS
%left '-'
%left '+'
%left MOD_OPERATOR
%left '\\'
%left '/'
%left '*'
%nonassoc UNARY_MINUS
%left '^'

%nonassoc VARIABLE_REDUCE
%nonassoc '('

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
    STATEMENT                               { $$ = new std::vector<std::unique_ptr<ast::Expression>>(); $$->push_back(std::unique_ptr<ast::Expression>($1)); }
|   NOT_EMPTY_STATEMENT_LIST ':' STATEMENT  { $$ = $1; $$->push_back(std::unique_ptr<ast::Expression>($3)); }

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
    VARIABLE %prec VARIABLE_REDUCE  { $$ = $1; }
|   VARIABLE '(' EXP_LIST ')'       { $$ = new ast::VectorGetElementExpression(move_ptr($1), move_ptr($3)); }

LVALUE_LIST:
    LVALUE                  { $$ = new std::vector<std::unique_ptr<ast::Expression>>(); $$->push_back(std::unique_ptr<ast::Expression>($1)); }
|   LVALUE_LIST ',' LVALUE  { $$ = $1; $$->push_back(std::unique_ptr<ast::Expression>($3)); }

PRINT_LIST:
    %empty                                 { $$ = new ast::PrintExpression(); }
|   PRINT_LIST EXP %prec LOWER_THAN_MINUS  { $$ = $1; $$->addExpression(std::unique_ptr<ast::Expression>($2)); $$->printNewLine = true; }
|   PRINT_LIST ';'                         { $$ = $1; $$->printNewLine = false; }
|   PRINT_LIST ','                         { $$ = $1; $$->addExpression(std::make_unique<ast::StringConstExpression>("    ")); $$->printNewLine = false; }

OPTIONAL_SEMICOLON:
    %empty
|   ';'

OPTIONAL_LINE_INPUT_PROMPT_STRING:
    %empty            { $$ = new ast::StringConstExpression(""); }
|   STRING_CONST ';'  { $$ = $1; }

OPTIONAL_INPUT_PROMPT_STRING:
    %empty            { $$ = new ast::StringConstExpression("? "); }
|   STRING_CONST ';'  { $$ = ast::asFunction("sum", move_vector<ast::Expression>({$1, new ast::StringConstExpression("? ")})).release(); }
|   STRING_CONST ','  { $$ = $1; }

LINE_NUMBER_LIST:
    LINE_NUMBER                       { $$ = new std::vector<int>(); $$->push_back($1); }
|   LINE_NUMBER_LIST ',' LINE_NUMBER  { $$ = $1; $$->push_back($3); }

EXP:
    NUM_CONST                  { $$ = $1; }
|   LINE_NUMBER                { if ($1 < 32768) $$ = new ast::IntConstExpression($1); else $$ = new ast::FloatConstExpression($1); }
|   STRING_CONST               { $$ = $1; }
|   LVALUE                     { $$ = $1; }
|   '(' EXP ')'                { $$ = $2; }
|   EXP '^' EXP                { $$ = ast::asFunction("pow", move_vector<ast::Expression>({$1, $3})).release(); }
|   '-' EXP %prec UNARY_MINUS  { $$ = ast::asFunction("neg", move_vector<ast::Expression>({$2})).release(); }
|   EXP '*' EXP                { $$ = ast::asFunction("mul", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '/' EXP                { $$ = ast::asFunction("fdiv", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '\\' EXP               { $$ = ast::asFunction("idiv", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP MOD_OPERATOR EXP       { $$ = ast::asFunction("mod", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '+' EXP                { $$ = ast::asFunction("sum", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP '-' EXP                { $$ = ast::asFunction("sub", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP EQUAL_OPERATOR EXP          { $$ = ast::asFunction("equal", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP UNEQUAL_OPERATOR EXP        { $$ = ast::asFunction("unequal", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP LESS_OPERATOR EXP           { $$ = ast::asFunction("less", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP GREATER_OPERATOR EXP        { $$ = ast::asFunction("greater", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP LESS_EQUAL_OPERATOR EXP     { $$ = ast::asFunction("geq", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP GREATER_EQUAL_OPERATOR EXP  { $$ = ast::asFunction("leq", move_vector<ast::Expression>({$1, $3})).release(); }
|   NOT_OPERATOR EXP                { $$ = ast::asFunction("not", move_vector<ast::Expression>({$2})).release(); }
|   EXP AND_OPERATOR EXP            { $$ = ast::asFunction("and", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP OR_OPERATOR EXP             { $$ = ast::asFunction("or", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP XOR_OPERATOR EXP            { $$ = ast::asFunction("xor", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP EQV_OPERATOR EXP            { $$ = ast::asFunction("eqv", move_vector<ast::Expression>({$1, $3})).release(); }
|   EXP IMP_OPERATOR EXP            { $$ = ast::asFunction("imp", move_vector<ast::Expression>({$1, $3})).release(); }
|   GW_FN_NAME '(' EXP_LIST ')'     { $$ = ast::asFunction(move_ptr($1), move_ptr($3)).release(); }

STATEMENT:
    OPTIONAL_LET_KEYWORD LVALUE EQUAL_OPERATOR EXP  { $$ = ast::asFunction("let", move_vector<ast::Expression>({$2, $4})).release(); }
|   DIM_KEYWORD VARIABLE '(' EXP_LIST ')'           { $$ = new ast::VectorDimExpression(move_ptr($2), move_ptr($4)); }
|   TRON_KEYWORD                                    { $$ = ast::asFunction("tron", {}).release(); }
|   TROFF_KEYWORD                                   { $$ = ast::asFunction("troff", {}).release(); }
|   PRINT_KEYWORD PRINT_LIST                        { $$ = $2; }
|   LINE_INPUT_KEYWORD OPTIONAL_SEMICOLON OPTIONAL_LINE_INPUT_PROMPT_STRING LVALUE  { $$ = ast::asFunction("lineinput", move_vector<ast::Expression>({$3, $4})).release(); }
|   INPUT_KEYWORD OPTIONAL_SEMICOLON OPTIONAL_INPUT_PROMPT_STRING LVALUE_LIST       { $$ = new ast::InputExpression(std::unique_ptr<ast::Expression>($3), move_ptr($4)); }
|   GOTO_KEYWORD LINE_NUMBER                        { $$ = new ast::GotoExpression($2); }
|   ON_KEYWORD EXP GOTO_KEYWORD LINE_NUMBER_LIST    { $$ = new ast::OnGotoExpression(std::unique_ptr<ast::Expression>($2), move_ptr($4)); }
