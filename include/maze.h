#ifndef MAZESOLVER_MAZE_H
#define MAZESOLVER_MAZE_H

#include <stdbool.h>

#ifndef WALL_SIZE
#define WALL_SIZE 15
#endif // !WALL_SIZE

typedef enum wall {
    WALL_NORTH = 1 << 0,
    WALL_SOUTH = 1 << 1,
    WALL_WEST = 1 << 2,
    WALL_EAST = 1 << 3,

    WALL_NOT_EMPTY = 1 << 4,

    WALL_ALL = WALL_NORTH | WALL_SOUTH | WALL_WEST | WALL_EAST
} wall_t;

typedef struct solver_state {
    int pos_x;
    int pos_y;

    int direction;
    int valid_directions;
} solver_state_t;

typedef struct maze {
    wall_t *walls;

    int width;
    int height;

    // Ponto de partida
    int start_x;
    int start_y;

    // Ponto de chegada
    int end_x;
    int end_y;

    bool solved;

    int solver_run_delay;
    double solver_current_time;

    struct {
        int top;
        int capacity;

        solver_state_t *states;
    } stack;
} maze_t;

// Inicializa o labirinto sem criá-lo.
void maze_init(maze_t *maze, int width, int height);

// Reverte a inicialização e também apaga o labirinto caso tenha sido criado.
void maze_deinit(maze_t *maze);

// Efetivamente cria o labirinto.
void maze_create(maze_t *maze);

// Retorna a largura e a altura do labirinto, respectivamente.
int maze_get_width(maze_t *maze);
int maze_get_height(maze_t *maze);

// Funções para definir, remover (respectivamente) as paredes de uma célula do
// labirinto.
void maze_set_wall(maze_t *maze, int x, int y, int wall);
void maze_remove_wall(maze_t *maze, int x, int y, int wall);
int maze_get_walls(maze_t *maze, int x, int y);

// Define a posição de início do labirinto
void maze_set_start(maze_t *maze, int x, int y);

// Atribui a x e y a posição de início do labirinto
// Retorna verdadeiro ou falso caso a posição de início foi definida
bool maze_get_start(maze_t *maze, int *x, int *y);

// Define a posição final do labirinto
void maze_set_end(maze_t *maze, int x, int y);

// Atribui a x e y a posição de fim do labirinto
// Retorna verdadeiro ou falso caso a posição de final foi definida
bool maze_get_end(maze_t *maze, int *x, int *y);

// Função usada para desenhar um labirinto na tela, ele é desenhado centralizado
// e usando a WALL_SIZE para o tamanho em pixels de cada parede.
void maze_draw(maze_t *maze);

// Inicia o solucionador de labirinto
void maze_solver_start(maze_t *maze);

// Remove os dados da inicialização do solucionador
void maze_solver_reset(maze_t *maze);

// Executa o solucionador até encontrar o ponto final
void maze_solver_run(maze_t *maze);

// Executa um passo em frente para encontrar o ponto final
void maze_solver_step_next(maze_t *maze);

// Retrocede um passo no processo de encontrar o ponto final
void maze_solver_step_previous(maze_t *maze);

// Retorna verdadeiro quando o caminho for achado
bool maze_solved(maze_t *maze);

#endif /* MAZESOLVER_MAZE_H */

