%{
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "burp2.h"
#include "parser.h"
#include "robot.h"
#ifdef YYDEBUG
int yydebug = 1;
#endif
int yyparse(void);
int yylex(void);
int yylex_destroy(void);
void yyerror(char* s);
int line_n = 0;
FILE* yyin;
robot* rob;
%}

%union {
    int ival;
    struct expression *exp;
    struct command *cmd;
    struct line *line;
};

%token NEGINT
%token POSINT

%token LPAR
%token RPAR

%token PLUS
%token MINUS
%token STAR
%token SLASH
%token PERC

%token LESSER
%token LEQUAL
%token EQUAL
%token NEQUAL
%token GEQUAL
%token GREATER

%token PEEK
%token RAND
%token CARDINAL
%token SELF
%token SPEED
%token STATE
%token GPSX
%token GPSY
%token ANGLE
%token TARGETX
%token TARGETY
%token DISTANCE

%token WAIT
%token POKE
%token GOTO
%token IF
%token THEN
%token ENGINE
%token SHOOT

%token LINEBR

%token END 0 "end of file"

%type<ival> NEGINT POSINT
%type<exp> condition expression integer number operation
%type<cmd> command
%type<line> line

%start program
%%

integer
    : NEGINT { $$ = make_expi($1); }
    | POSINT { $$ = make_expi($1); }
    ;

number
    : POSINT { $$ = make_expi($1); }
    ;

operation
    : LPAR expression PLUS expression RPAR {
        expression *e = make_expf(EXP_TYPE_ADD, 2);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $4, 1);
        $$ = e;
    }
    | LPAR expression MINUS expression RPAR {
        expression *e = make_expf(EXP_TYPE_SUB, 2);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $4, 1);
        $$ = e;
    }
    | LPAR expression STAR expression RPAR {
        expression *e = make_expf(EXP_TYPE_MUL, 2);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $4, 1);
        $$ = e;
    }
    | LPAR expression SLASH expression RPAR {
        expression *e = make_expf(EXP_TYPE_DIV, 2);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $4, 1);
        $$ = e;
    }
    | LPAR expression PERC expression RPAR {
        expression *e = make_expf(EXP_TYPE_MOD, 2);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $4, 1);
        $$ = e;
    }
    ;

condition
    : expression LESSER expression {
        expression *e = make_expf(EXP_TYPE_LESSER, 2);
        set_arg_exp(e, $1, 0);
        set_arg_exp(e, $3, 1);
        $$ = e;
    }
    | expression LEQUAL expression {
        expression *e = make_expf(EXP_TYPE_LEQUAL, 2);
        set_arg_exp(e, $1, 0);
        set_arg_exp(e, $3, 1);
        $$ = e;
    }
    | expression EQUAL expression {
        expression *e = make_expf(EXP_TYPE_EQUAL, 2);
        set_arg_exp(e, $1, 0);
        set_arg_exp(e, $3, 1);
        $$ = e;
    }
    | expression NEQUAL expression {
        expression *e = make_expf(EXP_TYPE_NEQUAL, 2);
        set_arg_exp(e, $1, 0);
        set_arg_exp(e, $3, 1);
        $$ = e;
    }
    | expression GEQUAL expression {
        expression *e = make_expf(EXP_TYPE_GEQUAL, 2);
        set_arg_exp(e, $1, 0);
        set_arg_exp(e, $3, 1);
        $$ = e;
    }
    | expression GREATER expression {
        expression *e = make_expf(EXP_TYPE_GREATER, 2);
        set_arg_exp(e, $1, 0);
        set_arg_exp(e, $3, 1);
        $$ = e;
    }
    ;

expression
    : integer
    | operation
    | PEEK expression {
        expression *e = make_expf(EXP_TYPE_PEEK, 1);
        set_arg_exp(e, $2, 0);
        $$ = e;
    }
    | RAND expression {
        expression *e = make_expf(EXP_TYPE_RAND, 1);
        set_arg_exp(e, $2, 0);
        $$ = e;
    }
    | CARDINAL {
        $$ = make_expf(EXP_TYPE_CARDINAL, 0);
    }
    | SELF {
        $$ = make_expf(EXP_TYPE_SELF, 0);
    }
    | SPEED {
        $$ = make_expf(EXP_TYPE_SPEED, 0);
    }
    | STATE expression {
        expression *e = make_expf(EXP_TYPE_STATE, 1);
        set_arg_exp(e, $2, 0);
        $$ = e;
    }
    | GPSX expression {
        expression *e = make_expf(EXP_TYPE_GPSX, 1);
        set_arg_exp(e, $2, 0);
        $$ = e;
    }
    | GPSY expression {
        expression *e = make_expf(EXP_TYPE_GPSY, 1);
        set_arg_exp(e, $2, 0);
        $$ = e;
    }
    | ANGLE expression expression expression expression {
        expression *e = make_expf(EXP_TYPE_ANGLE, 4);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $3, 1);
        set_arg_exp(e, $4, 2);
        set_arg_exp(e, $5, 3);
        $$ = e;
    }
    | TARGETX expression expression expression {
        expression *e = make_expf(EXP_TYPE_TARGETX, 3);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $3, 1);
        set_arg_exp(e, $4, 2);
        $$ = e;
    }
    | TARGETY expression expression expression {
        expression *e = make_expf(EXP_TYPE_TARGETY, 3);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $3, 1);
        set_arg_exp(e, $4, 2);
        $$ = e;
    }
    | DISTANCE expression expression expression expression {
        expression *e = make_expf(EXP_TYPE_DISTANCE, 4);
        set_arg_exp(e, $2, 0);
        set_arg_exp(e, $3, 1);
        set_arg_exp(e, $4, 2);
        set_arg_exp(e, $5, 3);
        $$ = e;
    }
    ;

command
    : WAIT expression {
        command *c = make_command(CMD_TYPE_WAIT, 1);
        set_arg_cmd(c, $2, 0);
        $$ = c;
    }
    | POKE expression expression {
        command *c = make_command(CMD_TYPE_POKE, 2);
        set_arg_cmd(c, $2, 0);
        set_arg_cmd(c, $3, 1);
        $$ = c;
    }
    | GOTO number {
        command *c = make_command(CMD_TYPE_GOTO, 1);
        set_arg_cmd(c, $2, 0);
        $$ = c;
    }
    | IF condition THEN number {
        command *c = make_command(CMD_TYPE_IFTHEN, 2);
        set_arg_cmd(c, $2, 0);
        set_arg_cmd(c, $4, 1);
        $$ = c;
    }
    | ENGINE expression expression {
        command *c = make_command(CMD_TYPE_ENGINE, 2);
        set_arg_cmd(c, $2, 0);
        set_arg_cmd(c, $3, 1);
        $$ = c;
    }
    | SHOOT expression expression {
        command *c = make_command(CMD_TYPE_SHOOT, 2);
        set_arg_cmd(c, $2, 0);
        set_arg_cmd(c, $3, 1);
        $$ = c;
    }
    ;

line
    : number command LINEBR {
        expression *num = $1;
        command *cmd = $2;
        if (num->val != line_n) {
            printf("line number is %d, should be %d\n", num->val, line_n);
            exit(-1);
        }
        line_n++;
        $$ = make_line(num->val, cmd);
    }
    ;

program
    : line program {
        addline($1, rob);
    }
    | END {
        printf("end of input\n");
    }
    ;

%%

void yyerror(char *s) {
    printf("yyerror : %s\n",s);
    exit(-1);
}

void parse(const char* file_name, robot* r) {
    yyin = fopen(file_name, "r");
    if (yyin == NULL) {
        perror(strerror(errno));
        exit(-1);
    }
    rob = r;
    printf("parsing %s\n", file_name);
    yyparse();
    fclose(yyin);
    yyin = NULL;
    line_n = 0;
}
