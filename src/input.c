#include "input.h"
#include "raylib.h"
#include "audio.h"

void ProcessInput(GameState* state) {
    switch (state->state) {
        case GAME_STATE_MENU:
            if (IsKeyPressed(KEY_SPACE)) {
                StartNewGame(state);
            }
            break;
            
        case GAME_STATE_PLAYING:
            state->ship.isThrusting = false;
            state->ship.rotationSpeed = 0;
            
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                ThrustSpaceship(&state->ship);
                PlayThrustSound();
            } else {
                StopThrustSound();
            }
            
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                RotateSpaceship(&state->ship, -1);
            }
            
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                RotateSpaceship(&state->ship, 1);
            }
            
            if (IsKeyPressed(KEY_SPACE)) {
                FireBullet(state);
            }
            
            if (IsKeyPressed(KEY_H)) {
                HyperspaceJump(&state->ship, state->screenWidth, state->screenHeight);
                PlayHyperspaceSound();
            }
            
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                PauseGame(state);
            }
            break;
            
        case GAME_STATE_PAUSED:
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
                ResumeGame(state);
            }
            break;
            
        case GAME_STATE_GAME_OVER:
            if (IsKeyPressed(KEY_SPACE)) {
                state->showingHighScore = false;
                StartNewGame(state);
            }
            break;
    }
}