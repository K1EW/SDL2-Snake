#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>

enum DIRECTION {
    DOWN,
    LEFT,
    UP,
    RIGHT
};

struct coordinate {
    int x;
    int y;
};
typedef struct coordinate Coordinate;

struct snake {
    Coordinate coord;
    unsigned int dir;
    struct snake* next;
};
typedef struct snake Snake;

struct apple {
    Coordinate coord;
};
typedef struct apple Apple;

void init_snake();
void create_apple();
void draw_apple(unsigned int x, unsigned int y);
void draw_grid(unsigned int x, unsigned int y);
void draw_snake(unsigned int x, unsigned int y);
void move_chunk(Snake *chunk, unsigned int next_dir);
void move_snake();
void grow_snake();
void check_game_over();
