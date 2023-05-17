#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "arena.h"
#include "robot.h"
#include "burp2.h"

expression *make_expi(int val) {
    expression *e = malloc(3 * sizeof(int));
    if (e == NULL) return NULL;
    e->type = EXP_TYPE_INT;
    e->val = val;
    e->n_args = 0;
    return e;
}

expression *make_expf(exp_type type, int n_args) {
    expression *e = malloc((sizeof(int) * 3) + (sizeof(expression*) * n_args));
    if (e == NULL) return NULL;
    e->type = type;
    e->val = 0;
    e->n_args = n_args;
    for (size_t i = 0; i < e->n_args; i++) {
        e->args[i] = NULL;
    }
    return e;
}

int set_arg_exp(expression *fun, expression *arg, int i) {
    if (fun == NULL || arg == NULL) return 0;
    if (fun->type == EXP_TYPE_INT) return 0;
    if (i < 0 || i >= fun->n_args) return 0;
    fun->args[i] = arg;
    return 1;
}

int evaluate(expression *e, robot *r, arena *a) {
    switch (e->type) {
        case EXP_TYPE_INT:
            return e->val;
        case EXP_TYPE_ADD:
            return evaluate(e->args[0], r, a) + evaluate(e->args[1], r, a);
        case EXP_TYPE_SUB:
            return evaluate(e->args[0], r, a) - evaluate(e->args[1], r, a);
        case EXP_TYPE_MUL:
            return evaluate(e->args[0], r, a) * evaluate(e->args[1], r, a);
        case EXP_TYPE_DIV:
            return evaluate(e->args[0], r, a) / evaluate(e->args[1], r, a);
        case EXP_TYPE_MOD:
            return evaluate(e->args[0], r, a) % evaluate(e->args[1], r, a);
        case EXP_TYPE_LESSER:
            return evaluate(e->args[0], r, a) < evaluate(e->args[1], r, a);
        case EXP_TYPE_LEQUAL:
            return evaluate(e->args[0], r, a) <= evaluate(e->args[1], r, a);
        case EXP_TYPE_EQUAL:
            return evaluate(e->args[0], r, a) == evaluate(e->args[1], r, a);
        case EXP_TYPE_NEQUAL:
            return evaluate(e->args[0], r, a) != evaluate(e->args[1], r, a);
        case EXP_TYPE_GEQUAL:
            return evaluate(e->args[0], r, a) >= evaluate(e->args[1], r, a);
        case EXP_TYPE_GREATER:
            return evaluate(e->args[0], r, a) > evaluate(e->args[1], r, a);
        case EXP_TYPE_PEEK:
            return exp_peek(evaluate(e->args[0], r, a), r);
        case EXP_TYPE_RAND:
            return exp_rand(evaluate(e->args[0], r, a));
        case EXP_TYPE_CARDINAL:
            return a->n_bots;
        case EXP_TYPE_SELF:
            return r->id;
        case EXP_TYPE_SPEED:
            return r->speed;
        case EXP_TYPE_STATE:
            return exp_state(evaluate(e->args[0], r, a), r, a);
        case EXP_TYPE_GPSX:
            return exp_gpsx(evaluate(e->args[0], r, a), r, a);
        case EXP_TYPE_GPSY:
            return exp_gpsy(evaluate(e->args[0], r, a), r, a);
        case EXP_TYPE_ANGLE:
            return (int)angle(evaluate(e->args[0], r, a), evaluate(e->args[1], r, a), evaluate(e->args[2], r, a), evaluate(e->args[3], r, a));
        case EXP_TYPE_TARGETX:
            return (int)next_x(evaluate(e->args[0], r, a), evaluate(e->args[1], r, a), evaluate(e->args[2], r, a));
        case EXP_TYPE_TARGETY:
            return (int)next_y(evaluate(e->args[0], r, a), evaluate(e->args[1], r, a), evaluate(e->args[2], r, a));
        case EXP_TYPE_DISTANCE:
            return (int)distance(evaluate(e->args[0], r, a), evaluate(e->args[1], r, a), evaluate(e->args[2], r, a), evaluate(e->args[3], r, a));
    }
    return 0;
}

int exp_peek(int n, robot *r) {
    if (n < 0 || n >= MEM_SIZE) return 0;
    return r->memory[n];
}

int exp_rand(int n) {
    srand(time(NULL));
    return rand() % n;
}

int exp_state(int n, robot *r, arena *a) {
    if (n == r->id) return r->damage;
    robot *rb = get_robot(n, a);
    if (rb != NULL) return rb->damage;
    return 0;
}

int exp_gpsx(int n, robot *r, arena *a) {
    if (n == r->id) return r->x;
    robot *rb = get_robot(n, a);
    if (rb != NULL) return rb->x;
    return 0;
}

int exp_gpsy(int n, robot *r, arena *a) {
    if (n == r->id) return r->y;
    robot *rb = get_robot(n, a);
    if (rb != NULL) return rb->y;
    return 0;
}

command *make_command(cmd_type type, int n_args) {
    command *c = malloc((sizeof(int) * 2) + (sizeof(expression*) * n_args));
    if (c == NULL) return NULL;
    c->type = type;
    c->n_args = n_args;
    for (size_t i = 0; i < c->n_args; i++) {
        c->args[i] = NULL;
    }
    return c;
}

int set_arg_cmd(command *cmd, expression *arg, int i) {
    if (cmd == NULL || arg == NULL) return 0;
    if (i < 0 || i >= cmd->n_args) return 0;
    cmd->args[i] = arg;
    return 1;
}

int execute(command *c, robot *r, arena *a) {
    switch(c->type) {
        case CMD_TYPE_WAIT:
            return cmd_wait(evaluate(c->args[0], r, a), r);
        case CMD_TYPE_POKE:
            return cmd_poke(evaluate(c->args[0], r, a), evaluate(c->args[1], r, a), r);
        case CMD_TYPE_GOTO:
            return cmd_goto(evaluate(c->args[0], r, a), r);
        case CMD_TYPE_IFTHEN:
            return cmd_ifthen(evaluate(c->args[0], r, a), evaluate(c->args[1], r, a), r);
        case CMD_TYPE_ENGINE:
            return cmd_engine(evaluate(c->args[0], r, a), evaluate(c->args[1], r, a), r);
        case CMD_TYPE_SHOOT:
            return cmd_shoot(evaluate(c->args[0], r, a), evaluate(c->args[1], r, a), r);
    }
    return 0;
}

int cmd_wait(int n, robot *r) {
    if (n < 0) return 0;
    r->delay = n;
    return 1;
}

int cmd_poke(int i, int n, robot *r) {
    if (i < 0 || i >= MEM_SIZE) return 0;
    r->memory[i] = n;
    return 1;
}

int cmd_goto(int n, robot *r) {
    line *l = get_line_n(r, n);
    if (l == NULL) return 0;
    r->current = l;
    return 1;
}

int cmd_ifthen(int condition, int n, robot *r) {
    return condition ? cmd_goto(n, r) : 0;
}

int cmd_engine(int angle, int speed, robot *r) {
    r->angle = angle;
    if (speed < 0) r->speed = 0;
    else if (speed > 100) r->speed = 100;
    else r->speed = speed;
    return 1;
}

int cmd_shoot(int angle, int distance, robot *r) {
    if (distance < 0) distance = 0;
    else if (distance > MAX_MISSILE_DIST) distance = MAX_MISSILE_DIST;
    if (r->missiles[0] == NULL) {
        r->missiles[0] = missile_factory(r->x + (ROBOT_WIDTH / 2), r->y + (ROBOT_HEIGHT / 2) , angle, distance);
        return 1;
    }
    else if (r->missiles[1] == NULL) {
        r->missiles[1] = missile_factory(r->x + (ROBOT_WIDTH / 2), r->y + (ROBOT_HEIGHT / 2), angle, distance);
        return 1;
    }
    return 0;
}

line *make_line(int n, command *cmd) {
    line *l = malloc(sizeof(line));
    if (l == NULL) return NULL;
    l->n = n;
    l->cmd = cmd;
    l->next = NULL;
    return l;
}

int do_line(line *l, robot *r, arena *a) {
    return execute(l->cmd, r, a);
}
