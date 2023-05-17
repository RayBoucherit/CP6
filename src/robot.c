#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "burp2.h"
#include "arena.h"
#include "robot.h"

robot *robot_factory(int id) {
    robot *fighter = malloc(sizeof(robot));
    if (fighter == NULL) return NULL;
    fighter->program = NULL;
    fighter->current = NULL;
    fighter->id = id;
    fighter->x = 0;
    fighter->y = 0;
    fighter->angle = 0;
    fighter->damage = 0;
    fighter->speed = 0;
    fighter->delay = 0;
    memset(fighter->memory, 0, MEM_SIZE * sizeof(int));
    fighter->missiles[0] = NULL;
    fighter->missiles[1] = NULL;
    return fighter;
}

missile* missile_factory(double x, double y, double angle, double distance) {
    missile* m = malloc(sizeof(missile));
    if (m == NULL) return NULL;
    m->x = x;
    m->y = y;
    m->angle = angle;
    m->distance = distance;
    return m;
}

int is_dead(robot* r) {
    return r->damage >= 100 ? 1 : 0;
}

void do_current(robot* r, arena* a) {
    if (r->current != NULL) {
        line *current = r->current;
        do_line(r->current, r, a);
        if (current == r->current) {
            if (r->current->next != NULL) r->current = r->current->next;
            else r->current = r->program;
        }
    }
}

int addline(line *l, robot *r) {
    if (l == NULL && r == NULL) return 0;
    if (r->program == NULL && r->current == NULL) {
        r->program = l;
        r->current = r->program;
        return 1;
    }
    line *ll = r->program;
    l->next = ll;
    r->program = l;
    r->current = r->program;
    return 1;
}

line* get_line_n(robot* r, int n) {
    if (r != NULL) {
        line* l = r->program;
        while (l->n != n && l) {
            l = l->next;
        }
        return l;
    }
    return NULL;
}

void hit(robot* r, int damage) {
    if (r->damage < 100) {
        r->damage += damage;
        if (r->damage > 100) {
            r->speed = 0;
        }
    }
}
