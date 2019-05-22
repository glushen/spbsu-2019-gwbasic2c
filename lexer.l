%option noyywrap nodefault yylineno case-insensitive

%{
#include <parser.h>
%}

NUM_WITH_DOT    (\-?[0-9]*([0-9]\.|\.[0-9])[0-9]*)
FN_NAME         (fn[a-z0-9\.]*)
VAR_NAME        ([a-z][a-z0-9\.]*)

%%

\'.+            { yylval.comment = strdup(yytext+1); return COMMENT; }
^\ *[0-9]{1,5}  { yylval.line_number = atoi(yytext); return LINE_NUMBER; }

\"[^\"]*\"      { yylval.exp = new ast::StringConstExpression(std::string(yytext+1, yyleng-2)); return CONST; }
\-?[0-9]{1,7}   { yylval.exp = new ast::FloatConstExpression(atoi(yytext)); return CONST; }
\-?[0-9]{8,}    { yylval.exp = new ast::DoubleConstExpression(atof(yytext)); return CONST; }
{NUM_WITH_DOT}  {
                    auto digit_count = yyleng - 1 - (yytext[0] == '-' ? 1 : 0);
                    if (digit_count <= 7) {
                        yylval.exp = new ast::FloatConstExpression(atof(yytext));
                    } else {
                        yylval.exp = new ast::DoubleConstExpression(atof(yytext));
                    }
                    return CONST;
                }
{NUM_WITH_DOT}E\-?[0-9]+ { yylval.exp = new ast::FloatConstExpression(atof(yytext)); return CONST; }
{NUM_WITH_DOT}D\-?[0-9]+ {
                             int i = 0;
                             while (yytext[i] != 'D' && yytext[i] != 'd') i++;
                             char temp = yytext[i];
                             yytext[i] = 'E';
                             yylval.exp = new ast::DoubleConstExpression(atof(yytext));
                             yytext[i] = temp;
                             return CONST;
                         }
{NUM_WITH_DOT}\! { yylval.exp = new ast::FloatConstExpression(atof(yytext)); return CONST; }
{NUM_WITH_DOT}\# { yylval.exp = new ast::DoubleConstExpression(atof(yytext)); }
&H[0-9a-f]{1,7}  { yylval.exp = new ast::FloatConstExpression(strtol(yytext+2, nullptr, 16)); return CONST; }
&H[0-9a-f]{8,}   { yylval.exp = new ast::DoubleConstExpression(strtoll(yytext+2, nullptr, 16)); return CONST; }
&O[0-9a-f]{1,7}  { yylval.exp = new ast::FloatConstExpression(strtol(yytext+2, nullptr, 8)); return CONST; }
&O[0-9a-f]{8,}   { yylval.exp = new ast::DoubleConstExpression(strtoll(yytext+2, nullptr, 8)); return CONST; }
&[0-9a-f]{1,7}   { yylval.exp = new ast::FloatConstExpression(strtol(yytext+1, nullptr, 8)); return CONST; }
&[0-9a-f]{8,}    { yylval.exp = new ast::DoubleConstExpression(strtoll(yytext+1, nullptr, 8)); return CONST; }

CSRLIN|DATE$|TIME$|ENVIRON\$|ERDEV|ERDEV\$|ERR|ERL|INKEY$ {
    yylval.name = strndup(yytext, yyleng); return UNSUPPORTED_VAR;
}
EXTERR|FRE|INP|IOCTL\$|LPOS|PEEK|PEN|PLAY|PMAP|POINT|POS|SCREEN|STICK|TAB|TIMER|USR|VARPTR|VARPTR\$ {
    yylval.name = strdup(yytext); return GW_FN_NAME_UNSUPPORTED;
}
ABS|ASC|ATN|CDBL|CHR\$|CINT|COS|CSNG|CVD|CVI|CVS|EOF|EXP|FIX|HEX\$|INPUT\$|INSTR|INT|LEFT\$|LEN|LOC|LOF|LOG|MID\$|MKD\$|MKI\$|MKS\$|OCT\$|RIGHT\$|RND|SGN|SIN|SPACE\$|SPC|SQR|STR\$|STRING\$|TAN|VAL {
    yylval.name = strdup(yytext); return GW_FN_NAME;
}
AUTO|BLOAD|BSAVE|CHDIR|CONT|DELETE|EDIT|FILES|LIST|LLIST|LOAD|MERGE|MKDIR|KILL|NEW|PCOPY|RUN|SAVE|SYSTEM {
    yylval.name = strdup(yytext); return GW_CMD_NAME_UNSUPPORTED;
}
CLEAR|NAME|RENUM|RESET|RMDIR|TROFF|TRON {
    yylval.name = strdup(yytext); return GW_CMD_NAME;
}
BEEP|CALL|CHAIN|CIRCLE|CLS|COLOR|COM|COMMON|DEFINT|DEFDBL|DEFSNG|DEFSTR|DEF\ +SEG|DEF\ +USR|DRAW|ENVIRON|ERROR|FIELD|GET|IOCTL|KEY|LINE|LINE\ +INPUT|LINE\ +INPUT\#|LOCATE|LOCK|LPRINT|LPRINT\ +USING|LSET|ON\ +COM|ON\ +KEY|ON\ +PEN|ON\ +PLAY|ON\ +STRIG|ON\ +TIMER|ON\ +ERROR\ +GOTO|OPEN\ +\"COM|OPTION\ +BASE|OUT|PAINT|PALETTE|PALETTE\ +USING|PEN|PLAY|POKE|PRESET|PSET|PRINT\ +USING|PRINT\#|PRINT\#\ +USING|PUT|RSET|SCREEN|SHELL|SOUND|STRIG|TIME\$|UNLOCK|VIEW|VIEW\ +PRINT|WAIT|WIDTH|WINDOW {
    yylval.name = strdup(yytext); return GW_STM_NAME_UNSUPPORTED;
}
CLOSE|DATA|DATE\$|DEF\ +FN|DIM|END|ERASE|FOR|GOSUB\-RETURN|GOTO|IF|INPUT|INPUT\#|LET|MID\$|NEXT|ON\-GOSUB|ON\-GOTO|OPEN|PRINT|RANDOMIZE|READ|REM|RESTORE|RESUME|RETURN|STOP|SWAP|WHILE\-WEND|WRITE|WRITE\# {
    yylval.name = strdup(yytext); return GW_STM_NAME;
}

\^|\*|\/|\\|\+|\-|\(|\)  { return yytext[0]; }
MOD  { return MOD_OPERATOR; }

\=   { return EQUAL_OPERATOR; }
\<\> { return UNEQUAL_OPERATOR; }
\<   { return LESS_OPERATOR; }
\>   { return GREATER_OPERATOR; }
\<\= { return LESS_EQUAL_OPERATOR; }
\>\= { return GREATER_EQUAL_OPERATOR; }

NOT  { return NOT_OPERATOR; }
AND  { return AND_OPERATOR; }
OR   { return OR_OPERATOR; }
XOR  { return XOR_OPERATOR; }
EQV  { return EQV_OPERATOR; }
IMP  { return IMP_OPERATOR; }

{FN_NAME}       { yylval.name = strndup(yytext, yyleng); return FN_VAR; }
{VAR_NAME}      { yylval.name = strndup(yytext, yyleng); return FLOAT_VAR; }
{VAR_NAME}\!    { yylval.name = strndup(yytext, yyleng-1); return FLOAT_VAR; }
{VAR_NAME}\%    { yylval.name = strndup(yytext, yyleng-1); return INT_VAR; }
{VAR_NAME}\#    { yylval.name = strndup(yytext, yyleng-1); return DOUBLE_VAR; }
{VAR_NAME}\$    { yylval.name = strndup(yytext, yyleng-1); return STRING_VAR; }

[ \t]           { /* ignore */ }
\r\n|\r|\n      { return '\n'; }
\:              { return yytext[0]; }
<<EOF>>         { return END_OF_FILE; }
.               { yyerror("Mystery character %c", yytext[0]); }

%%
