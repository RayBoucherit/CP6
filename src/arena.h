#ifndef ARENA_H
#define ARENA_H

#ifndef M_PI
#define M_PI 3.1416
#endif

#define ARENA_WIDTH 10000
#define ARENA_HEIGHT 10000
#define MAX_ROBOT_SPEED 10.0
#define MISSILE_SPEED 500
#define MAX_MISSILE_DIST 7000
#define COLLISION_DMG 2

typedef struct robot robot;
typedef struct missile missile;
typedef struct arena arena;

struct arena {
    struct robot** bots;
    int n_bots;
    int width;
    int height;
};

arena* arena_factory(struct robot** bots, int n_bots);

double distance(double x1, double y1, double x2, double y2);
double angle(double x1, double y1, double x2, double y2);
double next_x(double x, double angle, double distance);
double next_y(double y, double angle, double distance);
int find_intersection(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, double *x, double *y);
int find_collision(missile *m, arena *a, double *x, double *y);
void update_robots(arena* a);
void update_missiles(arena* a);
void update_missile_position(missile *m, arena *a);
void explode_missile(missile* m, arena* a);
robot *get_robot(int n, arena *a);
#endif
