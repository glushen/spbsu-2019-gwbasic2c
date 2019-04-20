%{
#include <stdio.h>
extern int yylex(void);
void yyerror(const char *s);
%}

%%

test: ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}