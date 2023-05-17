#ifndef ROBOT_H
#define ROBOT_H

#define ROBOT_WIDTH 10
#define ROBOT_HEIGHT 10
#define MEM_SIZE 1024

typedef struct line line;
typedef struct arena arena;
typedef struct robot robot;
typedef struct missile missile;

struct robot {
    int memory[MEM_SIZE];
    missile* missiles[2];
    line *program;
    line *current;
    int id;
    double x, y;      // coordinates
    double angle;
    int damage;    // percentage
    int speed;     // percentage
    int delay;
};

struct missile {
    double x, y, angle, distance;
};

robot *robot_factory(int id);
missile *missile_factory(double x, double y, double angle, double distance);

int is_dead(robot* r);
void do_current(robot* r, arena* a);
int addline(line *l, robot *r);
line* get_line_n(robot* r, int n);
void hit(robot* r, int damage);
#endif
