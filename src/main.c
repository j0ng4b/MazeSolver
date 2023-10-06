#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "raylib.h"

enum telas {
    TELA_PRINCIPAL,
    TELA_GERA_LABIRINTO,
    TELA_RESOLVE_LABIRINTO,
} g_tela = TELA_PRINCIPAL;

Vector2 randomPos(int row, int col);

int main(void) {
    Color colorBackground = {4, 84, 224, 255};
    Color colorButtons = {179, 182, 188, 255};
    Color colorGreen = {37, 204, 68, 255};


    srand(time(NULL));

    InitWindow(GetScreenWidth(), GetScreenHeight(), "Maze Solver");
    InitAudioDevice();

    Image mapa = LoadImage("img/map.png"), play = LoadImage("img/playButton.png");
    Image personagem = LoadImage("img/mario.png"), princesa = LoadImage("img/princesa.png"), obstaculo = LoadImage("img/goomba.png");
    Texture2D mapaTexture = LoadTextureFromImage(mapa), playButton = LoadTextureFromImage(play);
    Texture2D personagemTexture = LoadTextureFromImage(personagem), chegadaTexture = LoadTextureFromImage(princesa), obstaculoTexture = LoadTextureFromImage(obstaculo);

    Vector2 posMouse, coorCircle = {GetScreenWidth() / 13, GetScreenHeight() / 7 + 280}, posPersonagem, posChegada, posObstaculo;

    Rectangle recReset = {GetScreenWidth() / 25, GetScreenHeight() / 7 + 350, 110, 40};

    int row = 4, col = 4;

    int mat[4][4];

    unsigned short go = 0, auxColor = 0, controllSetMouse = 0;

    char reset[8] = "Resetar";

    while (!WindowShouldClose()) {
        // Aperta m e altera entre o modo de tela cheia
        if (IsKeyPressed(KEY_M))
            ToggleFullscreen();

        posMouse = GetMousePosition();
        BeginDrawing();
        ClearBackground(colorBackground);
        // Map
        DrawTexture(mapaTexture, GetScreenWidth() / 6, GetScreenHeight() / 7, WHITE);
        // Colisão com o start
        if(CheckCollisionPointCircle(posMouse, coorCircle, 30.f)){
            controllSetMouse = 0;
            SetMouseCursor(4);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                if(!go){
                    while(1){
                        posPersonagem = randomPos(row, col);
                        posChegada = randomPos(row, col);
                        posObstaculo = randomPos(row, col);
                        if((posPersonagem.x != posChegada.x && posPersonagem.y != posChegada.y) && (posObstaculo.x != posPersonagem.x && posObstaculo.y != posPersonagem.y) && (posObstaculo.x != posChegada.x && posObstaculo.y != posChegada.y)){
                            break;
                        }
                    }
                    // Preenchendo a matriz
                    memset(mat, 0, sizeof(mat));
                    mat[(int) posPersonagem.x][(int) posPersonagem.y] = 1;
                    mat[(int) posChegada.x][(int) posChegada.y] = 2;
                    mat[(int) posObstaculo.x][(int) posObstaculo.y] = -1;
                    // Verificar a matriz no CMD
                    printf("\n");
                    for(int i = 0; i < 4; i++){
                        printf("|");
                        for(int j = 0; j < 4; j++){
                            printf("%5d", mat[i][j]);
                        }
                        printf("     |\n");
                    }
                    printf("\n");
                    go = 1;
                }
            }
        }else{
            if(controllSetMouse == 0){
                SetMouseCursor(1);
            }
        }
        // Colisão com o reset
        if(CheckCollisionPointRec(posMouse, recReset)){
            SetMouseCursor(4);
            controllSetMouse = 1;
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                go = 0;
                auxColor = 400;
            }
        }else{
            if(controllSetMouse == 1){
                SetMouseCursor(1);
            }
        }
        // Botão de resetar padrão
        if(auxColor > 0){
            DrawRectangleRounded(recReset, 0.5, 0, colorGreen);
            auxColor--;
        }else{
            DrawRectangleRounded(recReset, 0.5, 0, colorButtons);
        }
        DrawRectangleLinesEx(recReset, 2, BLACK);
        DrawText(reset, recReset.x + 15, recReset.y + 10, 20, BLACK);
        // Button Play
        if(go){
            // Personagens
            DrawTexture(personagemTexture, GetScreenWidth() / 6 + 7 + (170 * posPersonagem.y + 30), GetScreenHeight() / 7 + 6 + (140 * posPersonagem.x + 30), WHITE);
            DrawTexture(chegadaTexture, GetScreenWidth() / 6 + 7 + (170 * posChegada.y + 40), GetScreenHeight() / 7 + 6 + (140 * posChegada.x + 30), WHITE);
            DrawTexture(obstaculoTexture, GetScreenWidth() / 6 + 7 + (170 * posObstaculo.y + 40), GetScreenHeight() / 7 + 6 + (140 * posObstaculo.x + 30), WHITE);
            DrawCircleV(coorCircle, 27.f, colorGreen);
        }else{
            DrawCircleV(coorCircle, 27.f, colorButtons);
        }
        // Textura do botão play e borda
        DrawCircleLines(coorCircle.x, coorCircle.y, 30.f, BLACK);
        DrawTexture(playButton, coorCircle.x - 10, coorCircle.y - 15, WHITE);
        EndDrawing();
    }

    UnloadTexture(mapaTexture);
    UnloadTexture(playButton);
    UnloadTexture(personagemTexture);
    UnloadTexture(obstaculoTexture);
    UnloadTexture(chegadaTexture);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}


Vector2 randomPos(int row, int col){
    Vector2 vector;
    vector.x = rand() % row;
    vector.y = rand() % col;
    return vector;
}

