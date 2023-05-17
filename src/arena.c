#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "robot.h"
#include "arena.h"

arena* arena_factory(robot** bots, int n_bots) {
    if (bots == NULL) return NULL;
    arena* a = malloc(sizeof(arena));
    if (a == NULL) return NULL;
    a->bots = bots;
    a->n_bots = n_bots;
    a->width = ARENA_WIDTH;
    a->height = ARENA_HEIGHT;
    return a;
}

double distance(double x1, double y1, double x2, double y2) {
    return (int)(sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2)));
}

double angle(double x1, double y1, double x2, double y2) {
    return atan2(y2 - y1, x2 - x1) * 180 / M_PI;
}

double next_x(double x, double angle, double distance) {
    double rad = (M_PI / 180.0) * angle;
    return x + distance * cos(rad);
}

double next_y(double y, double angle, double distance) {
    double rad = (M_PI / 180.0) * angle;
    return y + distance * sin(rad);
}

int find_intersection(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, double *x, double *y) {
    double s1x, s1y, s2x, s2y;
    s1x = x1 - x0;
    s1y = y1 - y0;
    s2x = x3 - x2;
    s2y = y3 - y2;

    double s, t;
    s = (-s1y * (x0 - x2) + s1x * (y0 - y2)) / (-s2x * s1y + s1x * s2y);
    t = (s2x * (y0 - y2) - s2y * (x0 - x2)) / (-s2x * s1y + s1x * s2y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        if (x != NULL) *x = x0 + (t * s1x);
        if (y != NULL) *y = y0 + (t * s1y);
        return 1;
    }
    return 0;
}

int find_collision(missile *m, arena *a, double *x, double *y) {
    int collision = 0;
    double col_x, col_y;
    double x0, y0, x1, y1;
    double d = MISSILE_SPEED;
    if (m->distance < d) d = m->distance;
    x0 = m->x;
    y0 = m->y;
    x1 = next_x(m->x, m->angle, d);
    y1 = next_y(m->y, m->angle, d);
    // check collisions with walls
    if (find_intersection(x0, y0, x1, y1, 0, 0, ARENA_WIDTH, 0, &col_x, &col_y)) {
        collision = 1;
        if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
            *x = col_x;
            *y = col_y;
        }
    }
    if (find_intersection(x0, y0, x1, y1, 0, 0, 0, ARENA_HEIGHT, &col_x, &col_y)) {
        collision = 1;
        if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
            *x = col_x;
            *y = col_y;
        }
    }
    if (find_intersection(x0, y0, x1, y1, ARENA_WIDTH, 0, ARENA_WIDTH, ARENA_HEIGHT, &col_x, &col_y)) {
        collision = 1;
        if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
            *x = col_x;
            *y = col_y;
        }
    }
    if (find_intersection(x0, y0, x1, y1, 0, ARENA_HEIGHT, ARENA_WIDTH, ARENA_HEIGHT, &col_x, &col_y)) {
        collision = 1;
        if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
            *x = col_x;
            *y = col_y;
        }
    }
    // check collisions with robots
    for (size_t i = 0; i < a->n_bots; i++) {
        robot *r = a->bots[i];
        if (r->missiles[0] != m && r->missiles[1] != m) {
            if (find_intersection(x0, y0, x1, y1, r->x, r->y, r->x + ROBOT_WIDTH, r->y, &col_x, &col_y)) {
                collision = 1;
                if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
                    *x = col_x;
                    *y = col_y;
                }
            }
            if (find_intersection(x0, y0, x1, y1, r->x, r->y, r->x, r->y + ROBOT_HEIGHT, &col_x, &col_y)) {
                collision = 1;
                if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
                    *x = col_x;
                    *y = col_y;
                }
            }
            if (find_intersection(x0, y0, x1, y1, r->x + ROBOT_WIDTH, r->y, r->x + ROBOT_WIDTH, r->y + ROBOT_HEIGHT, &col_x, &col_y)) {
                collision = 1;
                if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
                    *x = col_x;
                    *y = col_y;
                }
            }
            if (find_intersection(x0, y0, x1, y1, r->x, r->y + ROBOT_HEIGHT, r->x + ROBOT_WIDTH, r->y + ROBOT_HEIGHT, &col_x, &col_y)) {
                collision = 1;
                if (distance(x0, y0, col_x, col_y) < distance(x0, y0, *x, *y)) {
                    *x = col_x;
                    *y = col_y;
                }
            }
        }
    }
    return collision;
}

void update_robots(arena* a) {
    for (size_t i = 0; i < a->n_bots; i++) {
        robot* r0 = a->bots[i];
        if (!is_dead(r0) && r0->speed > 0) {
            int collision = 0;
            double px, py; // robot's next planned position
            double vx, vy; // vector from robot's current position to p
            px = next_x(r0->x, r0->angle, MAX_ROBOT_SPEED * ((double)r0->speed / 100.0));
            py = next_y(r0->y, r0->angle, MAX_ROBOT_SPEED * ((double)r0->speed / 100.0));
            vx = px - r0->x;
            vy = py - r0->y;
            // check & resolve collisions on x-axis
            if (px < 0) {
                // collision with west wall
                collision = 1;
                px = 0;
            }
            if (px > ARENA_WIDTH - ROBOT_WIDTH) {
                // collision with east wall
                collision = 1;
                px = ARENA_WIDTH - ROBOT_WIDTH;
            }
            for (size_t j = 0; j < a->n_bots; j++) {
                if (j != i) {
                    double x_overlap, y_overlap;
                    robot *r1 = a->bots[j];
                    x_overlap = ROBOT_WIDTH - abs(r1->x - px);
                    y_overlap = ROBOT_WIDTH - abs(r1->y - py);
                    if (x_overlap > 0 && y_overlap > 0) {
                        // collision occured
                        collision = 1;
                        if (vx > 0) {
                            // move west
                            px -= x_overlap;
                        }
                        else if (vx < 0) {
                            // move east
                            px += x_overlap;
                        }
                        hit(r1, COLLISION_DMG);
                        r1->speed = 0;
                    }
                }
            }
            // check & resolve collisions on y-axis
            if (py < 0) {
                // collision with north wall
                collision = 1;
                py = 0;
            }
            if (py > ARENA_HEIGHT - ROBOT_HEIGHT) {
                // collision with south wall
                collision = 1;
                py = ARENA_HEIGHT - ROBOT_HEIGHT;
            }
            for (size_t j = 0; j < a->n_bots; j++) {
                if (j != i) {
                    double x_overlap, y_overlap;
                    robot *r1 = a->bots[j];
                    x_overlap = ROBOT_WIDTH - abs(r1->x - px);
                    y_overlap = ROBOT_WIDTH - abs(r1->y - py);
                    if (x_overlap > 0 && y_overlap > 0) {
                        // collision occured
                        collision = 1;
                        if (vy > 0) {
                            // move north
                            py -= y_overlap;
                        }
                        else if (vy < 0) {
                            px += y_overlap;
                        }
                        hit(r1, COLLISION_DMG);
                        r1->speed = 0;
                    }
                }
            }
            r0->x = px;
            r0->y = py;
            if (collision) {
                hit(r0, COLLISION_DMG);
                r0->speed = 0;
            }
        }
    }
}

void update_missiles(arena *a) {
    for (size_t i = 0; i < a->n_bots; i++) {
        robot *r = a->bots[i];
        if (r->missiles[0] != NULL) {
            update_missile_position(r->missiles[0], a);
            if (r->missiles[0]->distance <= 0) {
                explode_missile(r->missiles[0], a);
                free(r->missiles[0]);
                r->missiles[0] = NULL;
            }
        }
        if (r->missiles[1] != NULL) {
            update_missile_position(r->missiles[1], a);
            if (r->missiles[1]->distance <= 0) {
                explode_missile(r->missiles[1], a);
                free(r->missiles[1]);
                r->missiles[1] = NULL;
            }
        }
    }
}

void update_missile_position(missile *m, arena *a) {
    double x, y, d;
    d = MISSILE_SPEED;
    if (m->distance < d) d = m->distance;
    x = next_x(m->x, m->angle, d);
    y = next_y(m->y, m->angle, d);
    if (find_collision(m, a, &x, &y)) m->distance = 0;
    else m->distance -= d;
    m->x = x;
    m->y = y;
}

void explode_missile(missile* m, arena* a) {
    for (size_t i = 0; i < a->n_bots; i++) {
        robot* r = a->bots[i];
        double d = distance(m->x, m->y, r->x + (ROBOT_WIDTH / 2), r->y + (ROBOT_HEIGHT / 2));
        if (d < 50) hit(r, 10);
        else if (d < 200) hit(r, 5);
        else if (d < 400) hit(r, 3);
    }
}

robot *get_robot(int n, arena *a) {
    for (size_t i = 0; i < a->n_bots; i++) {
        robot *r = a->bots[i];
        if (r->id == n) return r;
    }
    return NULL;
}
