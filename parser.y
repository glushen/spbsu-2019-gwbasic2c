%code requires {
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <ast.h>

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s, ...);
void handleResult(std::vector<ast::Line*>* line_list);
}

%union {
    int line_number;
    char* comment;
    std::vector<ast::Line*>* line_list;
    ast::Line* line;
    std::vector<ast::Statement*>* statementList;
    ast::Statement* statement;
    char* name;
    ast::Expression* exp;
}

%token <exp> CONST VARIABLE
%token <name> GW_FN_NAME_UNSUPPORTED GW_CMD_NAME_UNSUPPORTED GW_STM_NAME_UNSUPPORTED
%token <name> GW_FN_NAME GW_CMD_NAME GW_STM_NAME
%token <name> FN_VAR UNSUPPORTED_VAR
%token LET_KEYWORD
%token MOD_OPERATOR
%token EQUAL_OPERATOR UNEQUAL_OPERATOR LESS_OPERATOR GREATER_OPERATOR LESS_EQUAL_OPERATOR GREATER_EQUAL_OPERATOR
%token NOT_OPERATOR AND_OPERATOR OR_OPERATOR XOR_OPERATOR EQV_OPERATOR IMP_OPERATOR
%token <line_number> LINE_NUMBER
%token <comment> COMMENT
%token END_OF_FILE

%type <comment> OPTIONAL_COMMENT
%type <line_list> PROGRAM LINE_LIST
%type <line> LINE
%type <statementList> STATEMENT_LIST NOT_EMPTY_STATEMENT_LIST
%type <statement> STATEMENT
%type <exp> EXP

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
    LINE_LIST END_OF_FILE { $$ = $1; handleResult($$); YYACCEPT; }

LINE_LIST:
    LINE                { $$ = new std::vector<ast::Line*>(); if ($1 != nullptr) $$->push_back($1); }
|   LINE_LIST '\n' LINE { $$ = $1; if ($3 != nullptr) $$->push_back($3); }

LINE:
    %empty                                      { $$ = nullptr; }
|   LINE_NUMBER STATEMENT_LIST OPTIONAL_COMMENT { $$ = new ast::Line($1, $2, $3); }

STATEMENT_LIST:
    %empty                   { $$ = new std::vector<ast::Statement*>(); }
|   NOT_EMPTY_STATEMENT_LIST { $$ = $1; }

NOT_EMPTY_STATEMENT_LIST:
    STATEMENT                               { $$ = new std::vector<ast::Statement*>(); $$->push_back($1); }
|   NOT_EMPTY_STATEMENT_LIST ':' STATEMENT  { $$ = $1; $$->push_back($3); }

STATEMENT:
    EXP     { $$ = new ast::Statement($1); }

OPTIONAL_COMMENT:
    %empty           { $$ = strdup(""); }
|   COMMENT          { $$ = $1; }

OPTIONAL_LET_KEYWORD:
    %empty
|   LET_KEYWORD

EXP:
    CONST                      { $$ = $1; }
|   '(' EXP ')'                { $$ = $2; }
|   EXP '^' EXP                { $$ = ast::retrieveFunctionExpression("pow", {$1, $3}); }
|   '-' EXP %prec UNARY_MINUS  { $$ = ast::retrieveFunctionExpression("neg", {$2}); }
|   EXP '*' EXP                { $$ = ast::retrieveFunctionExpression("mul", {$1, $3}); }
|   EXP '/' EXP                { $$ = ast::retrieveFunctionExpression("fdiv", {$1, $3}); }
|   EXP '\\' EXP               { $$ = ast::retrieveFunctionExpression("idiv", {$1, $3}); }
|   EXP MOD_OPERATOR EXP       { $$ = ast::retrieveFunctionExpression("mod", {$1, $3}); }
|   EXP '+' EXP                { $$ = ast::retrieveFunctionExpression("sum", {$1, $3}); }
|   EXP '-' EXP                { $$ = ast::retrieveFunctionExpression("sub", {$1, $3}); }
|   EXP EQUAL_OPERATOR EXP          { $$ = ast::retrieveFunctionExpression("equal", {$1, $3}); }
|   EXP UNEQUAL_OPERATOR EXP        { $$ = ast::retrieveFunctionExpression("unequal", {$1, $3}); }
|   EXP LESS_OPERATOR EXP           { $$ = ast::retrieveFunctionExpression("less", {$1, $3}); }
|   EXP GREATER_OPERATOR EXP        { $$ = ast::retrieveFunctionExpression("greater", {$1, $3}); }
|   EXP LESS_EQUAL_OPERATOR EXP     { $$ = ast::retrieveFunctionExpression("geq", {$1, $3}); }
|   EXP GREATER_EQUAL_OPERATOR EXP  { $$ = ast::retrieveFunctionExpression("leq", {$1, $3}); }
|   NOT_OPERATOR EXP                { $$ = ast::retrieveFunctionExpression("not", {$2}); }
|   EXP AND_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("and", {$1, $3}); }
|   EXP OR_OPERATOR EXP             { $$ = ast::retrieveFunctionExpression("or", {$1, $3}); }
|   EXP XOR_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("xor", {$1, $3}); }
|   EXP EQV_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("eqv", {$1, $3}); }
|   EXP IMP_OPERATOR EXP            { $$ = ast::retrieveFunctionExpression("imp", {$1, $3}); }
|   OPTIONAL_LET_KEYWORD VARIABLE EQUAL_OPERATOR EXP  { $$ = ast::retrieveFunctionExpression("let", {$2, $4}); }
