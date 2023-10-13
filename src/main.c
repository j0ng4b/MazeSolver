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

// Variáveis globais
Texture g_icons_lucid;

int main(void)
{
    srand(time(NULL));

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(WINDOW_TARGET_FPS);

    g_icons_lucid = LoadTexture("assets/IconsLucid.png");

    maze_t maze;
    maze_init(&maze, 30, 30);
    maze_create(&maze);

    while (!WindowShouldClose()) {
        ClearBackground(BLACK);
        BeginDrawing();

        switch (g_screen) {
        case SCREEN_MENU:
            break;

        case SCREEN_MAZE:
            screen_maze(&maze);
            break;
        }

        EndDrawing();
    }

    UnloadTexture(g_icons_lucid);

    CloseWindow();
    return EXIT_SUCCESS;
}

void screen_maze(maze_t *maze)
{
    // Posições dos ícones da folha de sprites (spritesheet)
    static Rectangle magnifier    = { 64 * 6, 64 * 1, 64, 64 };
    static Rectangle insert_start = { 64 * 0, 64 * 1, 64, 64 };
    static Rectangle insert_end   = { 64 * 8, 64 * 4, 64, 64 };

    // Indica se está e o que está sendo inserido no labirinto:
    // 0 -> não está inserindo nada
    // 1 -> está inserindo um início
    // 2 -> está inserindo um fim
    static int inserting_location = 0;

    Color search_color;

    // Especifica a posição e o tamanho de uma imagem
    Rectangle dest = {
        .width = 32,
        .height = 32
    };

    maze_draw(maze);

    // Lupa
    dest.x = WINDOW_WIDTH - 42;
    dest.y = 10;

    if (maze_get_start(maze, NULL, NULL) && maze_get_end(maze, NULL, NULL))
        search_color = WHITE;
    else
        search_color = DARKGRAY;

    DrawTexturePro(g_icons_lucid, magnifier, dest, (Vector2) { 0, 0 }, 0, search_color);

    // Inserir início
    dest.x = WINDOW_WIDTH - 42;
    dest.y = 52;
    DrawTexturePro(g_icons_lucid, insert_start, dest, (Vector2) { 0, 0 }, 0, WHITE);


    // Inserir fim
    dest.x = WINDOW_WIDTH - 42;
    dest.y = 92;
    DrawTexturePro(g_icons_lucid, insert_end, dest, (Vector2) { 0, 0 }, 0, WHITE);

}

