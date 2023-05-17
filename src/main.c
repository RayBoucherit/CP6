#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <curses.h>
#include "burp2.h"
#include "parser.h"
#include "arena.h"
#include "robot.h"

#define CYCLE_LEN 100

void parse(const char* file_name, robot* r);

int main(int argc, char const *argv[]) {
    int running = 1;
    if (argc != 5) {
        printf("./main [path 1] [path 2] [path 3] [path 4]\n");
        exit(-1);
    }
    robot* bots[4];
    bots[0] = robot_factory(0);
    bots[0]->x = 0;
    bots[0]->y = 0;
    bots[1] = robot_factory(1);
    bots[1]->x = ARENA_WIDTH - ROBOT_WIDTH;
    bots[1]->y = 0;
    bots[2] = robot_factory(2);
    bots[2]->x = 0;
    bots[2]->y = ARENA_HEIGHT - ROBOT_HEIGHT;
    bots[3] = robot_factory(3);
    bots[3]->x = ARENA_WIDTH - ROBOT_WIDTH;
    bots[3]->y = ARENA_HEIGHT - ROBOT_HEIGHT;
    // TO DO: randomly assign starting position
    parse(argv[1], bots[0]);
    parse(argv[2], bots[1]);
    parse(argv[3], bots[2]);
    parse(argv[4], bots[3]);

    printf("%d\n", bots[3]->current->n);

    arena* a = arena_factory(bots, 4);

    initscr();
    WINDOW *warena, *warena_border;
    WINDOW *wrobot[4];
    warena_border = subwin(stdscr, 52, 82, 0, 0);
    box(warena_border, '#', '#');
    warena = subwin(warena_border, 50, 80, 1, 1);
    wrobot[0] = subwin(stdscr, 26, 41, 0, 83);
    wrobot[1] = subwin(stdscr, 26, 41, 0, 125);
    wrobot[2] = subwin(stdscr, 26, 41, 27, 83);
    wrobot[3] = subwin(stdscr, 26, 41, 27, 125);
    int tile_width = ARENA_WIDTH / 80;
    int tile_height = ARENA_HEIGHT / 50;
    refresh();

    // main loop
    while (1) {
        if (running) {
            werase(warena);
            for (size_t i = 0; i < a->n_bots; i++) {
                robot* r = a->bots[i];
                // update robot stats window
                WINDOW* w = wrobot[i];
                werase(w);
                wprintw(w, "ROBOT %d\n", r->id);
                wprintw(w, "X:%lf Y:%lf\n", r->x, r->y);
                wprintw(w, "SPEED:%d\n", r->speed);
                wprintw(w, "DAMAGE:%d\n", r->damage);
                wprintw(w, "LINE:%d\n", r->current->n);
                // show missiles
                missile *m0 = r->missiles[0];
                missile *m1 = r->missiles[1];
                wprintw(w, "MISSILE 0\n");
                if (m0 != NULL) {
                    wprintw(w, "X:%lf Y:%lf\nDISTANCE:%lf\n", m0->x, m0->y, m0->distance);
                    wmove(warena, m0->y / tile_height, m0->x / tile_width);
                    waddch(warena, '*');
                }
                else wprintw(w, "N/A\n");
                wprintw(w, "MISSILE 1\n");
                if (m1 != NULL) {
                    wprintw(w, "X:%lf Y:%lf\nDISTANCE:%lf\n", m1->x, m1->y, m1->distance);
                    wmove(warena, m1->y / tile_height, m1->x / tile_width);
                    waddch(warena, '*');
                }
                else wprintw(w, "N/A\n");
                wrefresh(w);
                // show robot in arena
                wmove(warena, r->y / tile_height, r->x / tile_width);
                waddch(warena, '@');
            }

            wrefresh(warena);

            int frags = 0;
            for (size_t i = 0; i < a->n_bots; i++) {
                if (is_dead(a->bots[i])) {
                    frags += 1;
                }
            }
            if (frags > 2) {
                running = 0;
            }

            for (size_t i = 0; i < a->n_bots; i++) {
                robot* r = a->bots[i];
                if (!is_dead(r)) {
                    if (r->delay > 0) r->delay -= 1;
                    else do_current(r, a);
                }
            }

            update_robots(a);
            update_missiles(a);

            usleep(CYCLE_LEN * 1000);
        }
    }

    return 0;
}
