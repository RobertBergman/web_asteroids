#include "raylib.h"
#include "game.h"
#include "input.h"
#include "audio.h"
#include <stdbool.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define TARGET_FPS 60

typedef struct {
    GameState* gameState;
    bool shouldClose;
} MainContext;

static MainContext mainCtx = {0};

void UpdateDrawFrame(void) {
    // Update screen dimensions if window was resized
    int currentWidth = GetScreenWidth();
    int currentHeight = GetScreenHeight();
    if (currentWidth != mainCtx.gameState->screenWidth || currentHeight != mainCtx.gameState->screenHeight) {
        mainCtx.gameState->screenWidth = currentWidth;
        mainCtx.gameState->screenHeight = currentHeight;
    }
    
    ProcessInput(mainCtx.gameState);
    
    UpdateGame(mainCtx.gameState, GetFrameTime());
    
    BeginDrawing();
        ClearBackground(BLACK);
        DrawGame(mainCtx.gameState);
    EndDrawing();
    
    if (WindowShouldClose()) {
        mainCtx.shouldClose = true;
    }
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Asteroids");
    SetTargetFPS(TARGET_FPS);
    
    InitAudioDevice();
    InitGameAudio();
    
    mainCtx.gameState = CreateGameState(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!mainCtx.gameState) {
        fprintf(stderr, "Failed to create game state\n");
        CloseWindow();
        return 1;
    }
    
    InitGame(mainCtx.gameState);
    
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!mainCtx.shouldClose && !WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif
    
    DestroyGameState(mainCtx.gameState);
    CloseGameAudio();
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}