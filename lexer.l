%option noyywrap nodefault yylineno

%{
#include <parser.h>
%}

NUM_WITH_DOT    (\-?[0-9]*([0-9]\.|\.[0-9])[0-9]*)
FN_NAME         ([Ff][Nn][a-zA-Z0-9\.]*)
VAR_NAME        ([a-zA-Z][a-zA-Z0-9\.]*)

%%

[a-zA-Z]+       { yylval.s = new std::string(yytext); return SOME_STRING; }

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

ABS|ASC|ATN|CDBL|CHR\$|CINT|COS|CSNG|CVD|CVI|CVS|ENVIRON\$|EOF|EXP|EXTERR|FIX|FRE|HEX\$|INP|INPUT\$|INSTR|INT|IOCTL\$|LEFT\$|LEN|LOC|LOF|LOG|LPOS|MID\$|MKD\$|MKI\$|MKS\$|OCT\$|PEEK|PEN|PLAY|PMAP|POINT|POS|RIGHT\$|RND|SCREEN|SGN|SIN|SPACE\$|SPC|SQR|STICK|STR\$|STRING\$|TAB|TAN|TIMER|USR|VAL|VARPTR|VARPTR\$ {
    yylval.s = new std::string(yytext); return GW_FN_NAME;
}
AUTO|BLOAD|BSAVE|CHDIR|CLEAR|CONT|DELETE|EDIT|FILES|KILL|LIST|LLIST|LOAD|MERGE|MKDIR|NAME|NEW|PCOPY|RENUM|RESET|RMDIR|RUN|SAVE|SYSTEM|TROFF|TRON {
    yylval.s = new std::string(yytext); return GW_CMD_NAME;
}
BEEP|CALL|CHAIN|CIRCLE|CLOSE|CLS|COLOR|COM|COMMON|DATA|DATE\$|DEF\ +FN|DEFINT|DEFDBL|DEFSNG|DEFSTR|DEF\ +SEG|DEF\ +USR|DIM|DRAW|END|ENVIRON|ERASE|ERROR|FIELD|FOR|GOSUB\-RETURN|GOTO|IF|INPUT|INPUT\#|IOCTL|KEY|LET|LINE|LINE\ +INPUT|LINE\ +INPUT\#|LOCATE|LOCK|LPRINT|LPRINT\ +USING|LSET|MID\$|NEXT|ON\ +COM|ON\ +KEY|ON\ +PEN|ON\ +PLAY|ON\ +STRIG|ON\ +TIMER|ON\ +ERROR\ +GOTO|ON\-GOSUB|ON\-GOTO|OPEN|OPEN\ +\"COM|OPTION\ +BASE|OUT|PAINT|PALETTE|PALETTE\ +USING|PEN|PLAY|POKE|PRESET|PSET|PRINT|PRINT\ +USING|PRINT\#|PRINT\#\ +USING|RANDOMIZE|READ|REM|RESTORE|RESUME|RETURN|RSET|SCREEN|SHELL|SOUND|STOP|STRIG|STRIG|SWAP|TIME\$|UNLOCK|VIEW|VIEW\ +PRINT|WAIT|WHILE\-WEND|WIDTH|WINDOW|WRITE|WRITE\# {
    yylval.s = new std::string(yytext); return GW_STM_NAME;
}

{FN_NAME}       { yylval.s = new std::string(yytext, yyleng); return FN_VAR; }
{VAR_NAME}      { yylval.s = new std::string(yytext, yyleng); return FLOAT_VAR; }
{VAR_NAME}\!    { yylval.s = new std::string(yytext, yyleng-1); return FLOAT_VAR; }
{VAR_NAME}\%    { yylval.s = new std::string(yytext, yyleng-1); return INT_VAR; }
{VAR_NAME}\#    { yylval.s = new std::string(yytext, yyleng-1); return DOUBLE_VAR; }
{VAR_NAME}\$    { yylval.s = new std::string(yytext, yyleng-1); return STRING_VAR; }

[ \t]           { /* ignore */ }
\r\n|\r|\n      { return '\n'; }
\:|\'           { return yytext[0]; }
<<EOF>>         { return END_OF_FILE; }
.               { yyerror("Mystery character %c", yytext[0]); }

%%
