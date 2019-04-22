%option noyywrap nodefault yylineno

%{
#include <parser.h>
%}

NUM_WITH_DOT    (\-?[0-9]*([0-9]\.|\.[0-9])[0-9]*)

%%

\"[^\"]*\"      { yylval.s = new std::string(yytext, 1, yyleng-2); return STRING_CONST; }
\-?[0-9]{1,7}   {
                    int value = atoi(yytext);
                    if (-32768 <= value && value <= 32767) {
                        yylval.i = value;
                        return INT_CONST;
                    } else {
                        yylval.f = (float) value;
                        return FLOAT_CONST;
                    }
                }
\-?[0-9]{8,}    { yylval.d = atof(yytext); return DOUBLE_CONST; }
{NUM_WITH_DOT}  {
                    auto digit_count = yyleng - 1 - (yytext[0] == '-' ? 1 : 0);
                    if (digit_count <= 7) {
                        yylval.f = (float) atof(yytext);
                        return FLOAT_CONST;
                    } else {
                        yylval.d = atof(yytext);
                        return DOUBLE_CONST;
                    }
                }
{NUM_WITH_DOT}E\-?[0-9]+ { yylval.f = (float) atof(yytext); return FLOAT_CONST; }
{NUM_WITH_DOT}D\-?[0-9]+ {
                             int i = 0;
                             while (yytext[i] != 'D') i++;
                             yytext[i] = 'E';
                             yylval.d = atof(yytext);
                             yytext[i] = 'D';
                             return DOUBLE_CONST;
                         }
{NUM_WITH_DOT}\!   { yylval.f = (float) atof(yytext); return FLOAT_CONST; }
{NUM_WITH_DOT}\#   { yylval.d = atof(yytext); return DOUBLE_CONST; }
&H[0-9a-fA-F]{1,7} { yylval.f = (float) strtol(yytext+2, nullptr, 16); return FLOAT_CONST; }
&H[0-9a-fA-F]{8,}  { yylval.d = (double) strtoll(yytext+2, nullptr, 16); return DOUBLE_CONST; }
&O[0-9a-fA-F]{1,7} { yylval.f = (float) strtol(yytext+2, nullptr, 8); return FLOAT_CONST; }
&O[0-9a-fA-F]{8,}  { yylval.d = (double) strtoll(yytext+2, nullptr, 8); return DOUBLE_CONST; }
&[0-9a-fA-F]{1,7}  { yylval.f = (float) strtol(yytext+1, nullptr, 8); return FLOAT_CONST; }
&[0-9a-fA-F]{8,}   { yylval.d = (double) strtoll(yytext+1, nullptr, 8); return DOUBLE_CONST; }

[ \t]           { /* ignore */ }
\r\n|\r|\n      { return '\n'; }
\:|\'           { return yytext[0]; }
[a-zA-Z]+       { yylval.s = new std::string(yytext); return SOME_STRING; }
.               { yyerror("Mystery character %c", yytext[0]); }
<<EOF>>         { return END_OF_FILE; }

%%
