%code requires {
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <string>
#include <ast.cpp>

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s, ...);
void handleResult(std::vector<Line*>* line_list);
}

%union {
    int i;
    float f;
    double d;
    std::string* s;
    std::vector<Line*>* line_list;
    Line* line;
    std::vector<Statement*>* statement_list;
    Statement* statement;
}

%token <s> SOME_STRING

%token <i> INT_CONST
%token <f> FLOAT_CONST
%token <d> DOUBLE_CONST
%token <s> STRING_CONST
%token <s> GW_FN_NAME GW_CMD_NAME GW_STM_NAME
%token <s> INT_VAR FLOAT_VAR DOUBLE_VAR STRING_VAR FN_VAR
%token END_OF_FILE

%type <i> LINE_NUMBER
%type <s> COMMENT
%type <line_list> PROGRAM LINE_LIST
%type <line> LINE
%type <statement_list> STATEMENT_LIST NOT_EMPTY_STATEMENT_LIST
%type <statement> STATEMENT

%%

PROGRAM:
    LINE_LIST END_OF_FILE { $$ = $1; handleResult($$); YYACCEPT; }

LINE_LIST:
    LINE                { $$ = new std::vector<Line*>(); if ($1 != nullptr) $$->push_back($1); }
|   LINE_LIST '\n' LINE { $$ = $1; if ($3 != nullptr) $$->push_back($3); };

LINE:
    %empty                             { $$ = nullptr; }
|   LINE_NUMBER STATEMENT_LIST COMMENT { $$ = new Line($1, $2, $3); };

LINE_NUMBER:
    INT_CONST { $$ = $1; };

STATEMENT_LIST:
    %empty                   { $$ = new std::vector<Statement*>(); }
|   NOT_EMPTY_STATEMENT_LIST { $$ = $1; };

NOT_EMPTY_STATEMENT_LIST:
    STATEMENT                    { $$ = new std::vector<Statement*>(); $$->push_back($1); }
|   STATEMENT_LIST ':' STATEMENT { $$ = $1; $$->push_back($3); };

STATEMENT:
    SOME_STRING { $$ = new Statement($1); };

COMMENT:
    %empty           { $$ = new std::string(); }
|   '\'' SOME_STRING { $$ = $2; }
