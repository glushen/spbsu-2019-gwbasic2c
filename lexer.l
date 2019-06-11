%option noyywrap nodefault yylineno case-insensitive

%{
#include <parser.h>
%}

NUM             ([0-9]*([0-9]\.|\.[0-9])?[0-9]*)
NUM_WITH_DOT    ([0-9]*([0-9]\.|\.[0-9])[0-9]*)
FN_NAME         (fn[a-z0-9\.]*)
VAR_NAME        ([a-z][a-z0-9\.]*)

%%

\'.+            { yylval.comment = new std::string(yytext+1); return COMMENT; }
REM\ .*         { yylval.comment = new std::string(yytext+4); return COMMENT; }
\"[^\"]*\"      { yylval.exp = new ast::StringConstExpression(std::string(yytext+1, yyleng-2)); return STRING_CONST; }
[0-9]{1,5}      { yylval.lineNumber = atoi(yytext); return LINE_NUMBER; }
[0-9]{1,7}      { yylval.exp = new ast::FloatConstExpression(atol(yytext)); return NUM_CONST; }
[0-9]{8,}       { yylval.exp = new ast::DoubleConstExpression(atof(yytext)); return NUM_CONST; }
{NUM_WITH_DOT}  {
                    auto digit_count = yyleng - 1 - (yytext[0] == '-' ? 1 : 0);
                    if (digit_count <= 7) {
                        yylval.exp = new ast::FloatConstExpression(atof(yytext));
                    } else {
                        yylval.exp = new ast::DoubleConstExpression(atof(yytext));
                    }
                    return NUM_CONST;
                }
{NUM}E\-?[0-9]+ { yylval.exp = new ast::FloatConstExpression(atof(yytext)); return NUM_CONST; }
{NUM}D\-?[0-9]+ {
                    int i = 0;
                    while (yytext[i] != 'D' && yytext[i] != 'd') i++;
                    char temp = yytext[i];
                    yytext[i] = 'E';
                    yylval.exp = new ast::DoubleConstExpression(atof(yytext));
                    yytext[i] = temp;
                    return NUM_CONST;
                }
{NUM}\!                  { yylval.exp = new ast::FloatConstExpression(atof(yytext)); return NUM_CONST; }
{NUM}\#                  { yylval.exp = new ast::DoubleConstExpression(atof(yytext)); return NUM_CONST; }
&H0*[0-7][0-9a-f]{1,3}   { yylval.exp = new ast::IntConstExpression(strtol(yytext+2, nullptr, 16)); return NUM_CONST; }
&H0*[89a-f][0-9a-f]{1,3} { yylval.exp = new ast::IntConstExpression(strtol(yytext+2, nullptr, 16) - 65536); return NUM_CONST; }
&O0*[0-7]{1,5}           { yylval.exp = new ast::IntConstExpression(strtol(yytext+2, nullptr, 8)); return NUM_CONST; }
&0*[0-7]{1,5}            { yylval.exp = new ast::IntConstExpression(strtol(yytext+1, nullptr, 8)); return NUM_CONST; }

LET    { return LET_KEYWORD; }
DIM    { return DIM_KEYWORD; }
TRON   { return TRON_KEYWORD; }
TROFF  { return TROFF_KEYWORD; }
PRINT  { return PRINT_KEYWORD; }
LINE\ +INPUT  { return LINE_INPUT_KEYWORD; }
INPUT  { return INPUT_KEYWORD; }
ON     { return ON_KEYWORD; }
GOTO   { return GOTO_KEYWORD; }
IF     { return IF_KEYWORD; }
THEN   { return THEN_KEYWORD; }
ELSE   { return ELSE_KEYWORD; }
WHILE  { return WHILE_KEYWORD; }
WEND   { return WEND_KEYWORD; }
SWAP   { return SWAP_KEYWORD; }
STOP   { return STOP_KEYWORD; }
END    { return END_KEYWORD; }
RANDOMIZE  { return RANDOMIZE_KEYWORD; }
RND    { return RND_KEYWORD; }
MID\$  { return MID_KEYWORD; }
ERASE  { return ERASE_KEYWORD; }
WRITE  { return WRITE_KEYWORD; }
FOR    { return FOR_KEYWORD; }
TO     { return TO_KEYWORD; }
STEP   { return STEP_KEYWORD; }
NEXT   { return NEXT_KEYWORD; }

CSRLIN|DATE$|TIME$|ENVIRON\$|ERDEV|ERDEV\$|ERR|ERL|INKEY$ {
    yylval.name = new std::string(yytext, yyleng); return UNSUPPORTED_VAR;
}
EXTERR|FRE|INP|IOCTL\$|LPOS|PEEK|PEN|PLAY|PMAP|POINT|POS|SCREEN|STICK|TAB|TIMER|USR|VARPTR|VARPTR\$ {
    yylval.name = new std::string(yytext); return GW_FN_NAME_UNSUPPORTED;
}
ABS|ASC|ATN|CDBL|CHR\$|CINT|COS|CSNG|CVD|CVI|CVS|EOF|EXP|FIX|HEX\$|INPUT\$|INSTR|INT|LEFT\$|LEN|LOC|LOF|LOG|MKD\$|MKI\$|MKS\$|OCT\$|RIGHT\$|SGN|SIN|SPACE\$|SPC|SQR|STR\$|STRING\$|TAN|VAL {
    yylval.name = new std::string(yytext); return GW_FN_NAME;
}
AUTO|BLOAD|BSAVE|CHDIR|CLEAR|CONT|DELETE|EDIT|FILES|KILL|LIST|LLIST|LOAD|MERGE|MKDIR|NAME|NEW|PCOPY|RENUM|RESET|RMDIR|RUN|SAVE|SYSTEM {
    yylval.name = new std::string(yytext); return GW_CMD_NAME_UNSUPPORTED;
}
BEEP|CALL|CHAIN|CIRCLE|CLOSE|CLS|COLOR|COM|COMMON|DATA|DATE\$|DEFINT|DEFDBL|DEFSNG|DEFSTR|DEF\ +SEG|DEF\ +USR|DRAW|ENVIRON|ERROR|FIELD|GET|IOCTL|KEY|LINE|LINE\ +INPUT\#|LOCATE|LOCK|LPRINT|LPRINT\ +USING|LSET|ON\ +COM|ON\ +KEY|ON\ +PEN|ON\ +PLAY|ON\ +STRIG|ON\ +TIMER|ON\ +ERROR\ +GOTO|OPEN|OPEN\ +\"COM|OPTION\ +BASE|OUT|PAINT|PALETTE|PALETTE\ +USING|PEN|PLAY|POKE|PRESET|PSET|PRINT\ +USING|PRINT\#|PRINT\#\ +USING|PUT|READ|RESTORE|RESUME|RSET|SCREEN|SHELL|SOUND|STRIG|TIME\$|UNLOCK|VIEW|VIEW\ +PRINT|WAIT|WIDTH|WINDOW|WRITE\#|DEF\ +FN|GOSUB\-RETURN|ON\-GOSUB|RETURN {
    yylval.name = new std::string(yytext); return GW_STM_NAME_UNSUPPORTED;
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

{FN_NAME}       { yylval.name = new std::string(yytext, yyleng); return FN_VAR; }
{VAR_NAME}      { yylval.exp = new ast::VariableExpression(std::string(yytext, yyleng), gw::FLOAT_REF); return VARIABLE; }
{VAR_NAME}\!    { yylval.exp = new ast::VariableExpression(std::string(yytext, yyleng-1), gw::FLOAT_REF); return VARIABLE; }
{VAR_NAME}\%    { yylval.exp = new ast::VariableExpression(std::string(yytext, yyleng-1), gw::INT_REF); return VARIABLE; }
{VAR_NAME}\#    { yylval.exp = new ast::VariableExpression(std::string(yytext, yyleng-1), gw::DOUBLE_REF); return VARIABLE; }
{VAR_NAME}\$    { yylval.exp = new ast::VariableExpression(std::string(yytext, yyleng-1), gw::STRING_REF); return VARIABLE; }

[ \t]           { /* ignore */ }
\r\n|\r|\n      { return '\n'; }
\:|\,|\;        { return yytext[0]; }
<<EOF>>         { return END_OF_FILE; }
.               { yyerror("Mystery character %c", yytext[0]); }

%%
