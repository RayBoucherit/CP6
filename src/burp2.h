#ifndef BURP_H
#define BURP_H



typedef struct robot robot;
typedef struct arena arena;

typedef struct expression expression;
typedef struct command command;
typedef struct line line;

typedef enum {
    EXP_TYPE_INT,
    EXP_TYPE_ADD,
    EXP_TYPE_SUB,
    EXP_TYPE_MUL,
    EXP_TYPE_DIV,
    EXP_TYPE_MOD,
    EXP_TYPE_LESSER,
    EXP_TYPE_LEQUAL,
    EXP_TYPE_EQUAL,
    EXP_TYPE_NEQUAL,
    EXP_TYPE_GEQUAL,
    EXP_TYPE_GREATER,
    EXP_TYPE_PEEK,
    EXP_TYPE_RAND,
    EXP_TYPE_CARDINAL,
    EXP_TYPE_SELF,
    EXP_TYPE_SPEED,
    EXP_TYPE_STATE,
    EXP_TYPE_GPSX,
    EXP_TYPE_GPSY,
    EXP_TYPE_ANGLE,
    EXP_TYPE_TARGETX,
    EXP_TYPE_TARGETY,
    EXP_TYPE_DISTANCE
} exp_type;

typedef enum {
    CMD_TYPE_WAIT,
    CMD_TYPE_POKE,
    CMD_TYPE_GOTO,
    CMD_TYPE_IFTHEN,
    CMD_TYPE_ENGINE,
    CMD_TYPE_SHOOT
} cmd_type;

struct expression {
    exp_type type;
    int val;
    int n_args;
    expression *args[];
};

struct command {
    cmd_type type;
    int n_args;
    expression *args[];
};

struct line {
    int n;
    command *cmd;
    line *next;
};

expression *make_expi(int val);
expression *make_expf(exp_type, int n_args);
int set_arg_exp(expression *fun, expression *arg, int i);
int evaluate(expression *e, robot *r, arena *a);
int exp_peek(int n, robot *r);
int exp_rand(int n);
int exp_state(int n, robot *r, arena *a);
int exp_gpsx(int n, robot *r, arena *a);
int exp_gpsy(int n, robot *r, arena *a);

command *make_command(cmd_type type, int n_args);
int set_arg_cmd(command *cmd, expression *arg, int i);
int execute(command *c, robot *r, arena *a);
int cmd_wait(int n, robot *r);
int cmd_poke(int i, int n, robot *r);
int cmd_goto(int n, robot *r);
int cmd_ifthen(int condition, int n, robot *r);
int cmd_engine(int angle, int speed, robot *r);
int cmd_shoot(int angle, int distance, robot *r);

line *make_line(int n, command *cmd);
int do_line(line *l, robot *r, arena *a);
#endif
