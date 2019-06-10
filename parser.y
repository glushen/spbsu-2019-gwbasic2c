%code requires {
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <memory>
#include <ast/program.h>
#include <parser_helper.h>

extern int yylex(void);
extern int yylineno;
void yyerror(const char *s, ...);
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
%token LET_KEYWORD DIM_KEYWORD TRON_KEYWORD TROFF_KEYWORD PRINT_KEYWORD LINE_INPUT_KEYWORD INPUT_KEYWORD
%token IF_KEYWORD THEN_KEYWORD ELSE_KEYWORD ON_KEYWORD GOTO_KEYWORD WHILE_KEYWORD WEND_KEYWORD
%token SWAP_KEYWORD STOP_KEYWORD END_KEYWORD RANDOMIZE_KEYWORD RND_KEYWORD MID_KEYWORD ERASE_KEYWORD
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
%type <expressions> STATEMENT_LIST NOT_EMPTY_STATEMENT_LIST THEN_STATEMENTS ELSE_STATEMENTS
%type <expressions> STATEMENT_SUBLIST DIM_LIST ERASE_LIST
%type <exp> EXP STATEMENT
%type <expressions> EXP_LIST NOT_EMPTY_EXP_LIST
%type <exp> LVALUE
%type <expressions> LVALUE_LIST
%type <printExp> PRINT_LIST
%type <exp> OPTIONAL_INPUT_PROMPT_STRING OPTIONAL_LINE_INPUT_PROMPT_STRING

%nonassoc LOWER_THAN_ELSE_AND_COLON
%nonassoc ELSE_KEYWORD
%nonassoc ':'

%nonassoc PRINT_KEYWORD

%nonassoc LOWER_THAN_COMMA
%nonassoc ','

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

%nonassoc LOWER_THAN_PARENTHESIS
%nonassoc '('

%%

PROGRAM:
    LINE_LIST END_OF_FILE { $$ = $1; ph::handleResult(ph::unwrap($$)); YYACCEPT; }

LINE_LIST:
    LINE                { $$ = new std::vector<ast::Line>(); if ($1 != nullptr) $$->push_back(ph::unwrap($1)); }
|   LINE_LIST '\n' LINE { $$ = $1; if ($3 != nullptr) $$->push_back(ph::unwrap($3)); }

LINE:
    %empty                                      { $$ = nullptr; }
|   LINE_NUMBER STATEMENT_LIST OPTIONAL_COMMENT { $$ = new ast::Line($1, ph::unwrap($2), ph::unwrap($3)); }

STATEMENT_LIST:
    %empty                   { $$ = ph::newExpressionVector(); }
|   NOT_EMPTY_STATEMENT_LIST { $$ = $1; }

NOT_EMPTY_STATEMENT_LIST:
    STATEMENT                                       { $$ = ph::newExpressionVector($1); }
|   STATEMENT_SUBLIST                               { $$ = $1; }
|   NOT_EMPTY_STATEMENT_LIST ':' STATEMENT          { $$ = $1; $$->emplace_back($3); }
|   NOT_EMPTY_STATEMENT_LIST ':' STATEMENT_SUBLIST  { $$ = $1; std::move($3->begin(), $3->end(), std::back_inserter(*$$)); delete $3; }

OPTIONAL_COMMENT:
    %empty           { $$ = new std::string; }
|   COMMENT          { $$ = $1; }

OPTIONAL_LET_KEYWORD:
    %empty
|   LET_KEYWORD

EXP_LIST:
    %empty              { $$ = ph::newExpressionVector(); }
|   NOT_EMPTY_EXP_LIST  { $$ = $1; }

NOT_EMPTY_EXP_LIST:
    EXP                         { $$ = ph::newExpressionVector($1); }
|   NOT_EMPTY_EXP_LIST ',' EXP  { $$ = $1; $$->emplace_back($3); }

LVALUE:
    VARIABLE %prec LOWER_THAN_PARENTHESIS  { $$ = $1; }
|   VARIABLE '(' EXP_LIST ')'              { $$ = new ast::VectorGetElementExpression(ph::unwrap($1), ph::unwrap($3)); }

LVALUE_LIST:
    LVALUE                  { $$ = ph::newExpressionVector($1); }
|   LVALUE_LIST ',' LVALUE  { $$ = $1; $$->emplace_back($3); }

PRINT_LIST:
    %empty                                 { $$ = new ast::PrintExpression(); }
|   PRINT_LIST EXP %prec LOWER_THAN_MINUS  { $$ = $1; $$->addExpression(ph::intoUniquePtr($2)); $$->printNewLine = true; }
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
|   STRING_CONST ';'  { $$ = ph::asFunction("sum", {$1, new ast::StringConstExpression("? ")}); }
|   STRING_CONST ','  { $$ = $1; }

LINE_NUMBER_LIST:
    LINE_NUMBER                       { $$ = new std::vector<int>(); $$->push_back($1); }
|   LINE_NUMBER_LIST ',' LINE_NUMBER  { $$ = $1; $$->push_back($3); }

OPTIONAL_COMMA:
    %empty %prec LOWER_THAN_COMMA
|   ','

THEN_STATEMENTS:
    GOTO_KEYWORD LINE_NUMBER                { $$ = ph::newExpressionVector(new ast::GotoExpression($2)); }
|   THEN_KEYWORD LINE_NUMBER                { $$ = ph::newExpressionVector(new ast::GotoExpression($2)); }
|   THEN_KEYWORD NOT_EMPTY_STATEMENT_LIST %prec LOWER_THAN_ELSE_AND_COLON  { $$ = $2; }

ELSE_STATEMENTS:
    %empty %prec LOWER_THAN_ELSE_AND_COLON  { $$ = ph::newExpressionVector(); }
|   ELSE_KEYWORD LINE_NUMBER                { $$ = ph::newExpressionVector(new ast::GotoExpression($2)); }
|   ELSE_KEYWORD NOT_EMPTY_STATEMENT_LIST   { $$ = $2; }

DIM_LIST:
    DIM_KEYWORD VARIABLE '(' EXP_LIST ')'   { $$ = ph::newExpressionVector(new ast::VectorDimExpression(ph::unwrap($2), ph::unwrap($4))); }
|   DIM_LIST ',' VARIABLE '(' EXP_LIST ')'  { $$ = $1; $$->emplace_back(new ast::VectorDimExpression(ph::unwrap($3), ph::unwrap($5))); }

ERASE_LIST:
    ERASE_KEYWORD VARIABLE                  { $$ = ph::newExpressionVector(new ast::VectorEraseExpression(ph::unwrap($2))); }
|   ERASE_LIST ',' VARIABLE                 { $$ = $1; $$->emplace_back(new ast::VectorEraseExpression(ph::unwrap($3))); }

EXP:
    NUM_CONST                  { $$ = $1; }
|   LINE_NUMBER                { if ($1 < 32768) $$ = new ast::IntConstExpression($1); else $$ = new ast::FloatConstExpression($1); }
|   STRING_CONST               { $$ = $1; }
|   LVALUE                     { $$ = $1; }
|   '(' EXP ')'                { $$ = $2; }
|   EXP '^' EXP                { $$ = ph::asFunction("pow", {$1, $3}); }
|   '-' EXP %prec UNARY_MINUS  { $$ = ph::asFunction("neg", {$2}); }
|   EXP '*' EXP                { $$ = ph::asFunction("mul", {$1, $3}); }
|   EXP '/' EXP                { $$ = ph::asFunction("fdiv", {$1, $3}); }
|   EXP '\\' EXP               { $$ = ph::asFunction("idiv", {$1, $3}); }
|   EXP MOD_OPERATOR EXP       { $$ = ph::asFunction("mod", {$1, $3}); }
|   EXP '+' EXP                { $$ = ph::asFunction("sum", {$1, $3}); }
|   EXP '-' EXP                { $$ = ph::asFunction("sub", {$1, $3}); }
|   EXP EQUAL_OPERATOR EXP          { $$ = ph::asFunction("equal", {$1, $3}); }
|   EXP UNEQUAL_OPERATOR EXP        { $$ = ph::asFunction("unequal", {$1, $3}); }
|   EXP LESS_OPERATOR EXP           { $$ = ph::asFunction("less", {$1, $3}); }
|   EXP GREATER_OPERATOR EXP        { $$ = ph::asFunction("greater", {$1, $3}); }
|   EXP LESS_EQUAL_OPERATOR EXP     { $$ = ph::asFunction("geq", {$1, $3}); }
|   EXP GREATER_EQUAL_OPERATOR EXP  { $$ = ph::asFunction("leq", {$1, $3}); }
|   NOT_OPERATOR EXP                { $$ = ph::asFunction("not", {$2}); }
|   EXP AND_OPERATOR EXP            { $$ = ph::asFunction("and", {$1, $3}); }
|   EXP OR_OPERATOR EXP             { $$ = ph::asFunction("or", {$1, $3}); }
|   EXP XOR_OPERATOR EXP            { $$ = ph::asFunction("xor", {$1, $3}); }
|   EXP EQV_OPERATOR EXP            { $$ = ph::asFunction("eqv", {$1, $3}); }
|   EXP IMP_OPERATOR EXP            { $$ = ph::asFunction("imp", {$1, $3}); }
|   GW_FN_NAME '(' EXP_LIST ')'     { $$ = ph::asFunction2(ph::unwrap($1), ph::unwrap($3)); }
|   RND_KEYWORD %prec LOWER_THAN_PARENTHESIS  { $$ = ph::asFunction("rnd", {new ast::IntConstExpression(1)}); }
|   RND_KEYWORD '(' EXP ')'                   { $$ = ph::asFunction("rnd", {$3}); }
|   MID_KEYWORD '(' EXP ',' EXP ',' EXP ')'   { $$ = ph::asFunction("mid$", {$3, $5, $7}); }
|   MID_KEYWORD '(' EXP ',' EXP ')'           { $$ = ph::asFunction("mid$", {$3, $5, new ast::IntConstExpression(255)}); }

STATEMENT:
    OPTIONAL_LET_KEYWORD LVALUE EQUAL_OPERATOR EXP  { $$ = ph::asFunction("let", {$2, $4}); }
|   TRON_KEYWORD                                    { $$ = ph::asFunction("tron", {}); }
|   TROFF_KEYWORD                                   { $$ = ph::asFunction("troff", {}); }
|   PRINT_KEYWORD PRINT_LIST                        { $$ = $2; }
|   LINE_INPUT_KEYWORD OPTIONAL_SEMICOLON OPTIONAL_LINE_INPUT_PROMPT_STRING LVALUE  { $$ = ph::asFunction("lineinput", {$3, $4}); }
|   INPUT_KEYWORD OPTIONAL_SEMICOLON OPTIONAL_INPUT_PROMPT_STRING LVALUE_LIST %prec LOWER_THAN_COMMA  { $$ = new ast::InputExpression(ph::intoUniquePtr($3), ph::unwrap($4)); }
|   GOTO_KEYWORD LINE_NUMBER                        { $$ = new ast::GotoExpression($2); }
|   ON_KEYWORD EXP GOTO_KEYWORD LINE_NUMBER_LIST %prec LOWER_THAN_COMMA          { $$ = new ast::OnGotoExpression(ph::intoUniquePtr($2), ph::unwrap($4)); }
|   IF_KEYWORD EXP OPTIONAL_COMMA THEN_STATEMENTS OPTIONAL_COMMA ELSE_STATEMENTS { $$ = new ast::IfExpression(ph::intoUniquePtr($2), ph::unwrap($4), ph::unwrap($6)); }
|   WHILE_KEYWORD EXP                               { $$ = new ast::WhileExpression(ph::intoUniquePtr($2)); }
|   WEND_KEYWORD                                    { $$ = new ast::WendExpression(); }
|   SWAP_KEYWORD LVALUE ',' LVALUE                  { $$ = ph::asFunction("swap", {$2, $4}); }
|   STOP_KEYWORD                                    { $$ = ph::asFunction("stop", {}); }
|   END_KEYWORD                                     { $$ = ph::asFunction("end", {}); }
|   RANDOMIZE_KEYWORD                               { $$ = ph::asFunction("randomize", {}); }
|   RANDOMIZE_KEYWORD EXP                           { $$ = ph::asFunction("randomize", {$2}); }
|   MID_KEYWORD '(' LVALUE ',' EXP ',' EXP ')' EQUAL_OPERATOR EXP  { $$ = ph::asFunction("mid$", {$3, $5, $7, $10}); }
|   MID_KEYWORD '(' LVALUE ',' EXP ')' EQUAL_OPERATOR EXP          { $$ = ph::asFunction("mid$", {$3, $5, new ast::IntConstExpression(255), $8}); }

STATEMENT_SUBLIST:
    DIM_LIST    { $$ = $1; } %prec LOWER_THAN_COMMA
|   ERASE_LIST  { $$ = $1; } %prec LOWER_THAN_COMMA