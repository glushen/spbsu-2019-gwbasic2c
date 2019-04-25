%option noyywrap nodefault yylineno case-insensitive

%{
#include <parser.h>
%}

NUM_WITH_DOT    (\-?[0-9]*([0-9]\.|\.[0-9])[0-9]*)
FN_NAME         (fn[a-z0-9\.]*)
VAR_NAME        ([a-z][a-z0-9\.]*)

%%

[a-z]+          { yylval.s = new std::string(yytext); return SOME_STRING; }
^\ *[0-9]{1,5}  { yylval.i = atoi(yytext); return LINE_NUMBER; }

\"[^\"]*\"      { yylval.s = new std::string(yytext, 1, yyleng-2); return STRING_CONST; }
\-?[0-9]{1,7}   { yylval.num_exp = new FloatConst(atoi(yytext)); return NUM_CONST; }
\-?[0-9]{8,}    { yylval.num_exp = new DoubleConst(atof(yytext)); return NUM_CONST; }
{NUM_WITH_DOT}  {
                    auto digit_count = yyleng - 1 - (yytext[0] == '-' ? 1 : 0);
                    if (digit_count <= 7) {
                        yylval.num_exp = new FloatConst(atoi(yytext));
                    } else {
                        yylval.num_exp = new DoubleConst(atof(yytext));
                    }
                    return NUM_CONST;
                }
{NUM_WITH_DOT}E\-?[0-9]+ { yylval.num_exp = new FloatConst(atof(yytext)); return NUM_CONST; }
{NUM_WITH_DOT}D\-?[0-9]+ {
                             int i = 0;
                             while (yytext[i] != 'D' && yytext[i] != 'd') i++;
                             char temp = yytext[i];
                             yytext[i] = 'E';
                             yylval.num_exp = new DoubleConst(atof(yytext));
                             yytext[i] = temp;
                             return NUM_CONST;
                         }
{NUM_WITH_DOT}\! { yylval.num_exp = new FloatConst(atof(yytext)); return NUM_CONST; }
{NUM_WITH_DOT}\# { yylval.num_exp = new DoubleConst(atof(yytext)); }
&H[0-9a-f]{1,7}  { yylval.num_exp = new FloatConst(strtol(yytext+2, nullptr, 16)); return NUM_CONST; }
&H[0-9a-f]{8,}   { yylval.num_exp = new DoubleConst(strtoll(yytext+2, nullptr, 16)); return NUM_CONST; }
&O[0-9a-f]{1,7}  { yylval.num_exp = new FloatConst(strtol(yytext+2, nullptr, 8)); return NUM_CONST; }
&O[0-9a-f]{8,}   { yylval.num_exp = new DoubleConst(strtoll(yytext+2, nullptr, 8)); return NUM_CONST; }
&[0-9a-f]{1,7}   { yylval.num_exp = new FloatConst(strtol(yytext+1, nullptr, 8)); return NUM_CONST; }
&[0-9a-f]{8,}    { yylval.num_exp = new DoubleConst(strtoll(yytext+1, nullptr, 8)); return NUM_CONST; }

ABS|ASC|ATN|CDBL|CHR\$|CINT|COS|CSNG|CVD|CVI|CVS|ENVIRON\$|EOF|EXP|EXTERR|FIX|FRE|HEX\$|INP|INPUT\$|INSTR|INT|IOCTL\$|LEFT\$|LEN|LOC|LOF|LOG|LPOS|MID\$|MKD\$|MKI\$|MKS\$|OCT\$|PEEK|PEN|PLAY|PMAP|POINT|POS|RIGHT\$|RND|SCREEN|SGN|SIN|SPACE\$|SPC|SQR|STICK|STR\$|STRING\$|TAB|TAN|TIMER|USR|VAL|VARPTR|VARPTR\$ {
    yylval.s = new std::string(yytext); return GW_FN_NAME;
}
AUTO|BLOAD|BSAVE|CHDIR|CLEAR|CONT|DELETE|EDIT|FILES|KILL|LIST|LLIST|LOAD|MERGE|MKDIR|NAME|NEW|PCOPY|RENUM|RESET|RMDIR|RUN|SAVE|SYSTEM|TROFF|TRON {
    yylval.s = new std::string(yytext); return GW_CMD_NAME;
}
BEEP|CALL|CHAIN|CIRCLE|CLOSE|CLS|COLOR|COM|COMMON|DATA|DATE\$|DEF\ +FN|DEFINT|DEFDBL|DEFSNG|DEFSTR|DEF\ +SEG|DEF\ +USR|DIM|DRAW|END|ENVIRON|ERASE|ERROR|FIELD|FOR|GOSUB\-RETURN|GOTO|IF|INPUT|INPUT\#|IOCTL|KEY|LET|LINE|LINE\ +INPUT|LINE\ +INPUT\#|LOCATE|LOCK|LPRINT|LPRINT\ +USING|LSET|MID\$|NEXT|ON\ +COM|ON\ +KEY|ON\ +PEN|ON\ +PLAY|ON\ +STRIG|ON\ +TIMER|ON\ +ERROR\ +GOTO|ON\-GOSUB|ON\-GOTO|OPEN|OPEN\ +\"COM|OPTION\ +BASE|OUT|PAINT|PALETTE|PALETTE\ +USING|PEN|PLAY|POKE|PRESET|PSET|PRINT|PRINT\ +USING|PRINT\#|PRINT\#\ +USING|RANDOMIZE|READ|REM|RESTORE|RESUME|RETURN|RSET|SCREEN|SHELL|SOUND|STOP|STRIG|STRIG|SWAP|TIME\$|UNLOCK|VIEW|VIEW\ +PRINT|WAIT|WHILE\-WEND|WIDTH|WINDOW|WRITE|WRITE\# {
    yylval.s = new std::string(yytext); return GW_STM_NAME;
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
