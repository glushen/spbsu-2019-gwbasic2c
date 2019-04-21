%option noyywrap nodefault yylineno

%{
#include <parser.h>
%}

%%

[0-9]+          { yylval.i = atoi(yytext); return INTEGER; }

[ \t]           { /* ignore */ }
\r\n|\r|\n      { return '\n'; }
\:|\'           { return yytext[0]; }
[a-zA-Z]+       { yylval.s = new std::string(yytext); return SOME_STRING; }
.               { yyerror("Mystery character %c", yytext[0]); }
<<EOF>>         { return END_OF_FILE; }

%%
