#include "maze.h"

#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "utils.h"

void maze_init(maze_t *maze, int width, int height)
{
    maze->walls = malloc(sizeof(wall_t) * width * height);
    memset(maze->walls, WALL_ALL, sizeof(wall_t) * width * height);

    maze->width = width;
    maze->height = height;
}

void maze_deinit(maze_t *maze)
{
    free(maze->walls);
}

void maze_create(maze_t *maze)
{
    int remaining = maze->width * maze->height - 1;

    // Posição inicial que será usada para criar um caminho no labirinto.
    int start_x;
    int start_y;

    // Posição atual.
    int cur_x;
    int cur_y;

    // Próxima...
    int next_x;
    int next_y;

    // Um array que armazena a sequencia de direções a partir de um ponto
    // inicial.
    int *dirs = malloc(sizeof(int) * maze->width * maze->height);
    int dir;

    // Tem um direção e tem isso aqui a direção oposta.
    int opposite_dir[] = { 1, 0, 3, 2 };

    maze_set_wall(maze, RANDINT(0, maze->width), RANDINT(0, maze->height), WALL_NOT_EMPTY);

    while (remaining > 0) {
        memset(dirs, -1, sizeof(int) * maze->width * maze->height);

        for (;;) {
            // Busca por uma célula vazia do labirinto
            cur_x = RANDINT(0, maze->width);
            cur_y = RANDINT(0, maze->height);
            if (maze_get_walls(maze, cur_x, cur_y) & WALL_NOT_EMPTY)
                continue;

            start_x = cur_x;
            start_y = cur_y;

            do {
                dir = rand() % 4;

                // Os cálculos abaixo é apenas para trocar intervalos,
                // basicamente, pega o intervalo [0, 1] e "projeta" em [-1, +1],
                // o mesmo para o intervalo [2, 3].
                next_x = cur_x + (dir > 1 ? (dir - 2) * 2 - 1 : 0);
                next_y = cur_y + (dir < 2 ? dir * 2 - 1 : 0);
                if (next_x < 0 || next_x >= maze->width || next_y < 0
                        || next_y >= maze->height)
                    continue;

                if (maze_get_walls(maze, cur_x, cur_y) & WALL_NOT_EMPTY)
                    break;

                dirs[cur_x + cur_y * maze->width] = dir;

                cur_x = next_x;
                cur_y = next_y;
            } while (true);

            break;
        }

        cur_x = start_x;
        cur_y = start_y;
        while ((dir = dirs[cur_x + cur_y * maze->width]) > -1) {
            next_x = cur_x + (dir > 1 ? (dir - 2) * 2 - 1: 0);
            next_y = cur_y + (dir < 2 ? dir * 2 - 1 : 0);

            // Diz que esse é uma célula válida do labirinto, que tem parede
            // nessa posição.
            maze_set_wall(maze, cur_x, cur_y, WALL_NOT_EMPTY);
            maze_set_wall(maze, next_x, next_y, WALL_NOT_EMPTY);

            // Remove as paredes para poder formar o caminho.
            maze_remove_wall(maze, cur_x, cur_y, 1 << dir);
            maze_remove_wall(maze, next_x, next_y, 1 << opposite_dir[dir]);

            cur_x = next_x;
            cur_y = next_y;

            remaining--;
        }
    }

    free(dirs);
}

void maze_set_wall(maze_t *maze, int x, int y, int wall)
{
    maze->walls[x + y * maze->width] |= wall;
}

void maze_remove_wall(maze_t *maze, int x, int y, int wall)
{
    maze->walls[x + y * maze->width] ^= wall;
}

int maze_get_walls(maze_t *maze, int x, int y)
{
    return maze->walls[x + y * maze->width];
}

static void draw_walls(int walls, Vector2 pos)
{
    Vector2 start_pos;
    Vector2 end_pos;

    if (walls & WALL_NORTH) {
        start_pos.x = pos.x;
        start_pos.y = pos.y;

        end_pos.x = start_pos.x + WALL_SIZE;
        end_pos.y = start_pos.y;
        DrawLineV(start_pos, end_pos, WHITE);
    }

    if (walls & WALL_SOUTH) {
        start_pos.x = pos.x;
        start_pos.y = pos.y;

        end_pos.x = start_pos.x + WALL_SIZE;
        end_pos.y = start_pos.y += WALL_SIZE;

        DrawLineV(start_pos, end_pos, WHITE);
    }

    if (walls & WALL_EAST) {
        start_pos.x = pos.x + WALL_SIZE;
        start_pos.y = pos.y;

        end_pos.x = pos.x + WALL_SIZE;
        end_pos.y = pos.y + WALL_SIZE;

        DrawLineV(start_pos, end_pos, WHITE);
    }

    if (walls & WALL_WEST) {
        start_pos.x = pos.x;
        start_pos.y = pos.y;

        end_pos.x = pos.x;
        end_pos.y = pos.y + WALL_SIZE;

        DrawLineV(start_pos, end_pos, WHITE);
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
            draw_walls(maze->walls[x + y * maze->width], pos);

        pos.y += WALL_SIZE;
    }
}

