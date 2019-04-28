%code requires {
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <ast.cpp>

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s, ...);
void handleResult(std::vector<Line*>* line_list);
}

%union {
    int line_number;
    char* comment;
    std::vector<Line*>* line_list;
    Line* line;
    std::vector<Statement*>* statement_list;
    Statement* statement;
    char* name;
    NumExp* num_exp;
    StringExp* string_exp;
}

%token <num_exp> NUM_CONST
%token <name> GW_FN_NAME_UNSUPPORTED GW_CMD_NAME_UNSUPPORTED GW_STM_NAME_UNSUPPORTED
%token <name> GW_FN_NAME GW_CMD_NAME GW_STM_NAME
%token <name> INT_VAR FLOAT_VAR DOUBLE_VAR STRING_VAR FN_VAR UNSUPPORTED_VAR
%token MOD_OPERATOR
%token EQUAL_OPERATOR UNEQUAL_OPERATOR LESS_OPERATOR GREATER_OPERATOR LESS_EQUAL_OPERATOR GREATER_EQUAL_OPERATOR
%token NOT_OPERATOR AND_OPERATOR OR_OPERATOR XOR_OPERATOR EQV_OPERATOR IMP_OPERATOR
%token <string_exp> STRING_CONST
%token <line_number> LINE_NUMBER
%token <comment> COMMENT
%token END_OF_FILE

%type <comment> OPTIONAL_COMMENT
%type <line_list> PROGRAM LINE_LIST
%type <line> LINE
%type <statement_list> STATEMENT_LIST NOT_EMPTY_STATEMENT_LIST
%type <statement> STATEMENT
%type <num_exp> NUM_EXP

%left '^'
%nonassoc UNARY_MINUS
%left '*'
%left '/'
%left '\\'
%left MOD_OPERATOR
%left '+'
%left '-'

%%

PROGRAM:
    LINE_LIST END_OF_FILE { $$ = $1; handleResult($$); YYACCEPT; }

LINE_LIST:
    LINE                { $$ = new std::vector<Line*>(); if ($1 != nullptr) $$->push_back($1); }
|   LINE_LIST '\n' LINE { $$ = $1; if ($3 != nullptr) $$->push_back($3); }

LINE:
    %empty                                      { $$ = nullptr; }
|   LINE_NUMBER STATEMENT_LIST OPTIONAL_COMMENT { $$ = new Line($1, $2, $3); }

STATEMENT_LIST:
    %empty                   { $$ = new std::vector<Statement*>(); }
|   NOT_EMPTY_STATEMENT_LIST { $$ = $1; }

NOT_EMPTY_STATEMENT_LIST:
    STATEMENT                               { $$ = new std::vector<Statement*>(); $$->push_back($1); }
|   NOT_EMPTY_STATEMENT_LIST ':' STATEMENT  { $$ = $1; $$->push_back($3); }

STATEMENT:
    NUM_EXP     { $$ = new Statement(strdup("{NUM_EXP}")); }

OPTIONAL_COMMENT:
    %empty           { $$ = strdup(""); }
|   COMMENT          { $$ = $1; }

NUM_EXP:
    NUM_CONST                      { $$ = $1; }
|   '(' NUM_EXP ')'                { $$ = $2; }
|   NUM_EXP '^' NUM_EXP            { $$ = new BinaryNumOp("pow", $1, $3); }
|   '-' NUM_EXP %prec UNARY_MINUS  { $$ = new UnaryNumOp("neg", $2); }
|   NUM_EXP '*' NUM_EXP            { $$ = new BinaryNumOp("mul", $1, $3); }
|   NUM_EXP '/' NUM_EXP            { $$ = new BinaryNumOp("fdiv", $1, $3); }
|   NUM_EXP '\\' NUM_EXP           { $$ = new BinaryNumOp("idiv", $1, $3); }
|   NUM_EXP MOD_OPERATOR NUM_EXP   { $$ = new BinaryNumOp("mod", $1, $3); }
|   NUM_EXP '+' NUM_EXP            { $$ = new BinaryNumOp("sum", $1, $3); }
|   NUM_EXP '-' NUM_EXP            { $$ = new BinaryNumOp("sub", $1, $3); }
