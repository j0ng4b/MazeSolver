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
    static Rectangle magnifier     = { 64 * 6, 64 * 1, 64, 64 };
    static Rectangle insert_start  = { 64 * 0, 64 * 1, 64, 64 };
    static Rectangle insert_end    = { 64 * 8, 64 * 4, 64, 64 };
    static Rectangle aim           = { 64 * 3, 64 * 0, 64, 64 };

    static Rectangle run           = { 64 * 4, 64 * 7, 64, 64 };
    static Rectangle pause         = { 64 * 8, 64 * 8, 64, 64 };
    static Rectangle stop_find     = { 64 * 9, 64 * 8, 64, 64 };
    static Rectangle step_next     = { 64 * 6, 64 * 5, 64, 64 };
    static Rectangle step_previous = { 64 * 5, 64 * 5, 64, 64 };

    // Indica se está e o que está sendo inserido no labirinto:
    // 0 -> não está inserindo nada
    // 1 -> está inserindo um início
    // 2 -> está inserindo um fim
    static int inserting_location = 0;

    static int inserting_pos_x;
    static int inserting_pos_y;

    static bool finding_solution = false;

    static bool running_solver = false;

    int current_inserted_x;
    int current_inserted_y;

    Color search_color;

    Rectangle maze_bounds = {
        .x = WINDOW_WIDTH / 2.0 - (maze_get_width(maze) * WALL_SIZE) / 2.0,
        .y = WINDOW_HEIGHT / 2.0 - (maze_get_height(maze) * WALL_SIZE) / 2.0,

        .width = maze_get_width(maze) * WALL_SIZE,
        .height = maze_get_height(maze) * WALL_SIZE
    };

    // Especifica a posição e o tamanho de uma imagem
    Rectangle dest = {
        .width = 32,
        .height = 32
    };

    if (running_solver)
        maze_solver_run(maze);

    maze_draw(maze);

    // Lupa
    dest.x = WINDOW_WIDTH - 42;
    dest.y = 10;

    if (maze_get_start(maze, NULL, NULL) && maze_get_end(maze, NULL, NULL)) {
        search_color = WHITE;

        if (CheckCollisionPointRec(GetMousePosition(), dest)
                && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            finding_solution = !finding_solution;
            inserting_location = 0;
            running_solver = 0;

            if (finding_solution)
                maze_solver_start(maze);
            else
                maze_solver_reset(maze);
        }
    } else {
        search_color = DARKGRAY;
    }

    if (finding_solution) {
        DrawTexturePro(g_icons_lucid, stop_find, dest,
            (Vector2) { 0, 0 }, 0, search_color);

        dest.width = dest.height = 22;

        // Botão de execução
        dest.x = WINDOW_WIDTH - 37;
        dest.y = 52;

        DrawTexturePro(g_icons_lucid, running_solver ? pause : run, dest,
            (Vector2) { 0, 0 }, 0, maze_solved(maze) ? GRAY : WHITE);

        if (!maze_solved(maze)
                && CheckCollisionPointRec(GetMousePosition(), dest)
                && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            running_solver = !running_solver;

        // Botão de avançar
        dest.x = WINDOW_WIDTH - 37;
        dest.y = 84;

        DrawTexturePro(g_icons_lucid, step_next, dest,
            (Vector2) { 0, 0 }, 0,
            running_solver || maze_solved(maze) ? GRAY : WHITE);

        if (!running_solver && !maze_solved(maze)
                && CheckCollisionPointRec(GetMousePosition(), dest)
                && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            maze_solver_step_next(maze);

        // Botão de retroceder
        dest.x = WINDOW_WIDTH - 37;
        dest.y = 116;

        DrawTexturePro(g_icons_lucid, step_previous, dest,
            (Vector2) { 0, 0 }, 0,
            running_solver || maze_solved(maze) ? GRAY : WHITE);

        if (!running_solver && !maze_solved(maze)
                && CheckCollisionPointRec(GetMousePosition(), dest)
                && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            maze_solver_step_previous(maze);

        // Não desenha nada abaixo
        return;
    }

    DrawTexturePro(g_icons_lucid, magnifier, dest,
        (Vector2) { 0, 0 }, 0, search_color);

    // Inserir início
    dest.x = WINDOW_WIDTH - 42;
    dest.y = 52;

    if (inserting_location == 1) {
        dest.x += 5;
        dest.y += 5;

        dest.width -= 10;
        dest.height -= 10;
    }

    DrawTexturePro(g_icons_lucid, insert_start, dest,
        (Vector2) { 0, 0 }, 0, WHITE);

    if (inserting_location == 1) {
        dest.x -= 5;
        dest.y -= 5;

        dest.width += 10;
        dest.height += 10;
    }

    if (CheckCollisionPointRec(GetMousePosition(), dest)
            && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        inserting_location = !(inserting_location == 1);

    // Inserir fim
    dest.x = WINDOW_WIDTH - 42;
    dest.y = 92;

    if (inserting_location == 2) {
        dest.x += 5;
        dest.y += 5;

        dest.width -= 10;
        dest.height -= 10;
    }

    DrawTexturePro(g_icons_lucid, insert_end, dest,
        (Vector2) { 0, 0 }, 0, WHITE);

    if (inserting_location == 2) {
        dest.x -= 5;
        dest.y -= 5;

        dest.width += 10;
        dest.height += 10;
    }

    if (CheckCollisionPointRec(GetMousePosition(), dest)
            && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        inserting_location = !(inserting_location == 2) * 2;

    if (inserting_location > 0
            && CheckCollisionPointRec(GetMousePosition(), maze_bounds)) {
        HideCursor();

        dest.width = dest.height = 16;
        dest.x = GetMouseX() - dest.width / 2.0;
        dest.y = GetMouseY() - dest.height / 2.0;

        DrawTexturePro(g_icons_lucid, aim, dest,
            (Vector2) { 0, 0 }, 0, WHITE);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            inserting_pos_x = (GetMouseX() - maze_bounds.x) / WALL_SIZE;
            inserting_pos_y = (GetMouseY() - maze_bounds.y) / WALL_SIZE;

            if (inserting_location == 1) {
                maze_get_start(maze, &current_inserted_x, &current_inserted_y);

                if (current_inserted_x == inserting_pos_x
                        && current_inserted_y == inserting_pos_y)
                    inserting_pos_x = inserting_pos_y = -1;

                maze_set_start(maze, inserting_pos_x, inserting_pos_y);
            } else {
                maze_get_end(maze, &current_inserted_x, &current_inserted_y);

                if (current_inserted_x == inserting_pos_x
                        && current_inserted_y == inserting_pos_y)
                    inserting_pos_x = inserting_pos_y = -1;

                maze_set_end(maze, inserting_pos_x, inserting_pos_y);
            }
        }
    } else {
        ShowCursor();
    }
}

