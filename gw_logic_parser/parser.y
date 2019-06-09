%code requires {
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <gw_logic_parser/main.h>

extern int yylex(void);
extern int yylex_destroy(void);
void yyerror(const char* str, ...);

extern int lexer_signature_offset;

template<typename T> T move_ptr(T* ptr) {
    T value = std::move(*ptr);
    delete ptr;
    return value;
}
}

%union {
    std::string* include_path;
    std::vector<std::string>* include_list;
    std::string* type_name;
    std::vector<std::string>* type_list;
    std::string* name;
}

%token INCLUDE_DIRECTIVE
%token <include_path> INCLUDE_PATH
%type <include_path> INCLUDE_LINE
%type <include_list> INCLUDE_LIST

%token <type_name> CONST_TYPE REF_TYPE VOID_TYPE
%type <type_name> ARGUMENT_TYPE RETURN_TYPE ARGUMENT
%type <type_list> ARGUMENT_LIST NON_EMPTY_ARGUMENT_LIST

%token <name> NAME

%%

FUNCTION_SIGNATURE_AND_DEPENDENCIES:
    INCLUDE_LIST RETURN_TYPE NAME '(' ARGUMENT_LIST ')' {
        yylex_destroy();
        handle_function_signature({move_ptr($1), move_ptr($2), move_ptr($3), move_ptr($5), lexer_signature_offset});
        YYACCEPT;
    }

INCLUDE_LIST:
    %empty                     { $$ = new std::vector<std::string>(); }
|   INCLUDE_LIST INCLUDE_LINE  { $$ = $1; $$->push_back(move_ptr($2)); }

INCLUDE_LINE:
    INCLUDE_DIRECTIVE INCLUDE_PATH END_OF_LINE  { $$ = $2; }

END_OF_LINE:
    '\n'
|   END_OF_LINE '\n'

ARGUMENT_TYPE:
    CONST_TYPE  { $$ = $1; }
|   REF_TYPE    { $$ = $1; }

RETURN_TYPE:
    CONST_TYPE  { $$ = $1; }
|   VOID_TYPE   { $$ = $1; }

ARGUMENT:
    ARGUMENT_TYPE NAME  { $$ = $1; }

ARGUMENT_LIST:
    %empty                   { $$ = new std::vector<std::string>(); }
|   NON_EMPTY_ARGUMENT_LIST  { $$ = $1; }

NON_EMPTY_ARGUMENT_LIST:
    ARGUMENT                              { $$ = new std::vector<std::string>(); $$->push_back(move_ptr($1)); }
|   NON_EMPTY_ARGUMENT_LIST ',' ARGUMENT  { $$ = $1; $$->push_back(move_ptr($3)); }
