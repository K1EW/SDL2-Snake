#include "snake.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define GRID_DIMENSION 30
#define GRID_SIZE 600

SDL_Window *window;
SDL_Renderer *renderer;
unsigned int x_offset = (WINDOW_WIDTH - GRID_SIZE) / 2;
unsigned int y_offset = (WINDOW_HEIGHT - GRID_SIZE) / 2;
bool running;

Snake *head;
Snake *tail;
unsigned int snake_length;
Apple *apple;

void setup() {
    srand(time(0));

    init_snake();
    create_apple();
}

void receive_input() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_DOWN:
                    if (head->dir == UP) continue;
                    head->dir = DOWN;
                    break;
                case SDLK_LEFT:
                    if (head->dir == RIGHT) continue;
                    head->dir = LEFT;
                    break;
                case SDLK_UP:
                    if (head->dir == DOWN) continue;
                    head->dir = UP;
                    break;
                case SDLK_RIGHT:
                    if (head->dir == LEFT) continue;
                    head->dir = RIGHT;
                    break;
            }
        }
    }
}

void update() {
    grow_snake();
    move_snake();
    check_game_over();
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    
    draw_apple(x_offset, y_offset);
    // draw_grid(x_offset, y_offset);
    draw_snake(x_offset, y_offset);

    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Snake",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    running = true;

    setup();
    
    while (running) {
        receive_input();
        update();
        render();

        SDL_Delay(80);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}

void init_snake() {
    head = malloc(sizeof(Snake));
    head->coord.x = rand() % (GRID_DIMENSION / 2) + (GRID_DIMENSION / 4);
    head->coord.y = rand() % (GRID_DIMENSION / 2) + (GRID_DIMENSION / 4);
    head->dir = UP;

    tail = malloc(sizeof(Snake));
    tail->coord.x = head->coord.x;
    tail->coord.y = head->coord.y + 1;
    tail->dir = UP;
    tail->next = NULL;

    head->next = tail;

    snake_length = 1;
}

void create_apple() {
    if (!apple) {
        apple = malloc(sizeof(Apple));
    }

    bool *grid = malloc(sizeof(bool) * GRID_DIMENSION * GRID_DIMENSION);
    memset(grid, true, sizeof(bool));

    Snake *trav = head;
    while (trav != NULL) {
        grid[trav->coord.y * GRID_DIMENSION + trav->coord.x] = false;
        trav = trav->next;
    }

    unsigned int available_coord_cap = GRID_DIMENSION * GRID_DIMENSION - snake_length;
    Coordinate *available_coord = malloc(sizeof(Coordinate) * available_coord_cap);
    int available_coord_size = 0;
    for (int x = 0; x < GRID_DIMENSION; x++) {
        for (int y = 0; y < GRID_DIMENSION; y++) {
            if (grid[y * GRID_DIMENSION + x]) {
                available_coord[available_coord_size].x = x;
                available_coord[available_coord_size].y = y;
                available_coord_size++;
            }
        }
    }

    Coordinate random_cord = available_coord[rand() % available_coord_size];
    apple->coord = random_cord;
}

void draw_apple(unsigned int x, unsigned int y) {
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0xff);

    unsigned int cell_size = GRID_SIZE / GRID_DIMENSION;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;
    cell.x = x + apple->coord.x * cell_size;
    cell.y = y + apple->coord.y * cell_size;
    SDL_RenderFillRect(renderer, &cell);
}

void draw_grid(unsigned int x, unsigned int y) {
    SDL_SetRenderDrawColor(renderer, 0x88, 0x88, 0x88, 0xff);
    
    unsigned int cell_size = GRID_SIZE / GRID_DIMENSION;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    for (int i = 0; i < GRID_DIMENSION; i++) {
        for (int j = 0; j < GRID_DIMENSION; j++) {
            cell.x = x + i * cell_size;
            cell.y = y + j * cell_size;
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
}

void draw_snake(unsigned int x, unsigned int y) {
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    
    unsigned int cell_size = GRID_SIZE / GRID_DIMENSION;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    Snake *trav = head;
    while(trav != NULL) {
        cell.x = x + trav->coord.x * cell_size;
        cell.y = y + trav->coord.y * cell_size;
        SDL_RenderFillRect(renderer, &cell);
        trav = trav->next;
    }
}

void move_chunk(Snake *chunk, unsigned int next_dir) {
    switch (chunk->dir) {
        case LEFT:
            chunk->coord.x--;
            break;
        case UP:
            chunk->coord.y--;
            break;
        case RIGHT:
            chunk->coord.x++;
            break;
        case DOWN:
            chunk->coord.y++;
            break;
    }
    chunk->dir = next_dir;
}

void move_snake() {
    unsigned int next_dir = head->dir;
    move_chunk(head, next_dir);
    
    Snake *trav = head->next;
    if (trav == NULL) {
        return;
    }

    while(trav != NULL) {
        unsigned int prev_dir = trav->dir;
        move_chunk(trav, next_dir);
        next_dir = prev_dir;
        trav = trav->next;
    }
}

void grow_snake() {
    if (head->coord.x == apple->coord.x && head->coord.y == apple->coord.y) {
        create_apple();

        Snake *new_chunk = malloc(sizeof(Snake));
        new_chunk->coord.x = tail->coord.x;
        new_chunk->coord.y = tail->coord.y + 1;
        new_chunk->dir = UP;
        new_chunk->next = NULL;
        tail->next = new_chunk;
        tail = new_chunk;
    }
}

void check_game_over() {
    if (head->coord.x >= 0 && head->coord.x < GRID_DIMENSION && head->coord.y >= 0 && head->coord.y < GRID_DIMENSION) {
        Snake *trav = head->next;
        if (trav == NULL) {
            return;
        }

        while (trav != NULL) {
            if (head->coord.x == trav->coord.x && head->coord.y == trav->coord.y) {
                running = false;
                break;
            }
            trav = trav->next;
        }
    } else {
        running = false;
    }
}
