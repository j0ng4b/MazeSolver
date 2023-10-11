#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define WINDOW_TITLE "MazeSolver"
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 480
#define WINDOW_TARGET_FPS 60

#define WALL_SIZE 15

enum screen {
    SCREEN_MENU,
    SCREEN_MAZE,
} g_screen = SCREEN_MAZE;

typedef enum wall {
    WALL_EMPTY = 0x00000,

    WALL_NORTH = 0x00001,
    WALL_SOUTH = 0x00010,
    WALL_EAST  = 0x00100,
    WALL_WEST  = 0x01000,

    WALL_ALL   = 0x01111,
    WALL_NO    = 0x10000,
} wall_t;

typedef struct maze {
    wall_t *walls;

    int width;
    int height;
} maze_t;

void maze_init(maze_t *maze, int width, int height);
void maze_create(maze_t *maze);
void maze_draw(maze_t *maze);

void screen_maze(maze_t *maze);

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);

    SetTargetFPS(WINDOW_TARGET_FPS);

    maze_t maze;
    maze_init(&maze, 30, 30);

    while (!WindowShouldClose()) {
        ClearBackground(BLACK);
        BeginDrawing();

        DrawFPS(0, 0);
        switch (g_screen) {
        case SCREEN_MENU:
            break;

        case SCREEN_MAZE:
            screen_maze(&maze);
            break;
        }

        EndDrawing();
    }

    CloseWindow();
    return EXIT_SUCCESS;
}

void screen_maze(maze_t *maze)
{
    maze_draw(maze);
}

void maze_init(maze_t *maze, int width, int height)
{
    maze->walls = malloc(sizeof(wall_t) * width * height);
    memset(maze->walls, WALL_EMPTY, sizeof(wall_t) * width * height);

    maze->width = width;
    maze->height = height;
}

static void draw_wall(wall_t wall, Vector2 pos)
{
    if (!(wall ^ WALL_ALL)) {
        Rectangle wall_rect = {
            .x = pos.x,
            .y = pos.y,

            .width = WALL_SIZE,
            .height = WALL_SIZE
        };

        DrawRectangleLinesEx(wall_rect, 1, WHITE);
    } else if (wall & WALL_NO) {
        Rectangle wall_rect = {
            .x = pos.x,
            .y = pos.y,

            .width = WALL_SIZE,
            .height = WALL_SIZE
        };

        DrawRectangleLinesEx(wall_rect, 1, PINK);
    } else {
        Vector2 start_pos;
        Vector2 end_pos;

        if (wall & WALL_NORTH) {
            start_pos.x = pos.x;
            start_pos.y = pos.y;

            end_pos.x = start_pos.x + WALL_SIZE;
            end_pos.y = start_pos.y;

            DrawLineV(start_pos, end_pos, WHITE);
        }

        if (wall & WALL_SOUTH) {
            start_pos.x = pos.x;
            start_pos.y = pos.y;

            end_pos.x = start_pos.x + WALL_SIZE;
            end_pos.y = start_pos.y += WALL_SIZE;

            DrawLineV(start_pos, end_pos, WHITE);
        }

        if (wall & WALL_EAST) {
            start_pos.x = pos.x;
            start_pos.y = pos.y;

            end_pos.x = pos.x;
            end_pos.y = pos.y + WALL_SIZE;

            DrawLineV(start_pos, end_pos, WHITE);
        }

        if (wall & WALL_WEST) {
            start_pos.x = pos.x + WALL_SIZE;
            start_pos.y = pos.y;

            end_pos.x = pos.x + WALL_SIZE;
            end_pos.y = pos.y + WALL_SIZE;

            DrawLineV(start_pos, end_pos, WHITE);
        }
    }
}

void maze_draw(maze_t *maze)
{
    Vector2 pos;

    Vector2 center = {
        .x = WINDOW_WIDTH / 2.0 - (maze->width * WALL_SIZE) / 2.0,
        .y = WINDOW_HEIGHT / 2.0 - (maze->height * WALL_SIZE) / 2.0
    };

    pos.y = center.y;
    for (int y = 0; y < maze->width; y++) {
        pos.x = center.x;

        for (int x = 0; x < maze->width; x++, pos.x += WALL_SIZE)
            draw_wall(maze->walls[x + y * maze->width], pos);

        pos.y += WALL_SIZE;
    }
}

