#include <stdlib.h>
#include <time.h>

#include "raylib.h"

#include "utils.h"
#include "maze.h"

#define WINDOW_TITLE "MazeSolver"

enum screen {
    SCREEN_MENU,
    SCREEN_MAZE,
} g_screen = SCREEN_MAZE;

/* Essa é a tela onde tem todas as opções para trabalhar no labirinto
*    Opções:
*      1. Novo labirinto
*      2. Salvar labirinto
*      3. Carregar labirinto
*      4. Definir pontos (inicio e chegada)
*      5. Resolver labirinto (é necessário que os pontos tenham sido definidos)
*        1. passo a passo
*        2. mostrar resposta direto
 */
void screen_maze(maze_t *maze);

int main(void)
{
    srand(time(NULL));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(WINDOW_TARGET_FPS);

    maze_t maze;
    maze_init(&maze, 30, 30);
    maze_create(&maze);

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

