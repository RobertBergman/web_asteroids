#include "game.h"
#include "utils.h"
#include "audio.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define INITIAL_ASTEROIDS 4
#define ASTEROIDS_PER_LEVEL 2
#define MAX_LEVEL_ASTEROIDS 12
#define UFO_BASE_SPAWN_TIME 20.0f
#define UFO_MIN_SPAWN_TIME 10.0f
#define FIRE_DELAY 0.25f
#define NEXT_LEVEL_DELAY 2.0f

GameState* CreateGameState(float screenWidth, float screenHeight) {
    GameState* state = calloc(1, sizeof(GameState));
    if (!state) return nullptr;
    
    state->screenWidth = screenWidth;
    state->screenHeight = screenHeight;
    state->highScore = 0;
    
    return state;
}

void DestroyGameState(GameState* state) {
    if (!state) return;
    
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (state->asteroids[i].shape) {
            free(state->asteroids[i].shape);
        }
    }
    
    free(state);
}

void InitGame(GameState* state) {
    state->state = GAME_STATE_MENU;
    state->showingHighScore = false;
}

void StartNewGame(GameState* state) {
    state->score = 0;
    state->level = 1;
    state->asteroidCount = 0;
    state->ufoSpawnTimer = 0;
    state->nextUFOSpawn = UFO_BASE_SPAWN_TIME;
    state->fireDelay = 0;
    state->nextLevelDelay = 0;
    state->respawnDelay = 0;
    
    InitSpaceship(&state->ship, state->screenWidth / 2, state->screenHeight / 2);
    
    memset(state->asteroids, 0, sizeof(state->asteroids));
    memset(state->bullets, 0, sizeof(state->bullets));
    memset(state->ufos, 0, sizeof(state->ufos));
    
    StartNewLevel(state);
    state->state = GAME_STATE_PLAYING;
}

void StartNewLevel(GameState* state) {
    int asteroidCount = INITIAL_ASTEROIDS + (state->level - 1) * ASTEROIDS_PER_LEVEL;
    if (asteroidCount > MAX_LEVEL_ASTEROIDS) {
        asteroidCount = MAX_LEVEL_ASTEROIDS;
    }
    
    SpawnAsteroids(state, asteroidCount);
    
    state->nextUFOSpawn = UFO_BASE_SPAWN_TIME - (state->level * 2);
    if (state->nextUFOSpawn < UFO_MIN_SPAWN_TIME) {
        state->nextUFOSpawn = UFO_MIN_SPAWN_TIME;
    }
    state->ufoSpawnTimer = 0;
}

void SpawnAsteroids(GameState* state, int count) {
    int spawned = 0;
    
    for (int i = 0; i < MAX_ASTEROIDS && spawned < count; i++) {
        if (!state->asteroids[i].isActive) {
            float x, y;
            do {
                x = RandomFloat(0, state->screenWidth);
                y = RandomFloat(0, state->screenHeight);
            } while (Vector2Distance((Vector2){x, y}, state->ship.position) < 100);
            
            InitAsteroid(&state->asteroids[i], x, y, ASTEROID_LARGE);
            spawned++;
            state->asteroidCount++;
        }
    }
}

void SpawnUFO(GameState* state) {
    for (int i = 0; i < MAX_UFOS; i++) {
        if (!state->ufos[i].isActive) {
            UFOType type = (state->score < 10000) ? UFO_LARGE : 
                          (RandomInt(0, 2) == 0 ? UFO_LARGE : UFO_SMALL);
            InitUFO(&state->ufos[i], type, state->screenWidth, state->screenHeight);
            PlayUFOSound();
            break;
        }
    }
}

void UpdateGame(GameState* state, float deltaTime) {
    switch (state->state) {
        case GAME_STATE_MENU:
            break;
            
        case GAME_STATE_PLAYING:
            UpdateSpaceship(&state->ship, deltaTime);
            WrapPosition(&state->ship.position, state->screenWidth, state->screenHeight);
            
            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                UpdateAsteroid(&state->asteroids[i], deltaTime, state->screenWidth, state->screenHeight);
            }
            
            for (int i = 0; i < MAX_BULLETS; i++) {
                UpdateBullet(&state->bullets[i], deltaTime, state->screenWidth, state->screenHeight);
            }
            
            for (int i = 0; i < MAX_UFOS; i++) {
                UpdateUFO(&state->ufos[i], deltaTime, &state->ship, state->bullets, MAX_BULLETS, state->screenWidth);
            }
            
            state->ufoSpawnTimer += deltaTime;
            if (state->ufoSpawnTimer > state->nextUFOSpawn) {
                SpawnUFO(state);
                state->ufoSpawnTimer = 0;
            }
            
            if (state->fireDelay > 0) {
                state->fireDelay -= deltaTime;
            }
            
            CheckCollisions(state);
            
            if (state->asteroidCount == 0) {
                state->nextLevelDelay += deltaTime;
                if (state->nextLevelDelay > NEXT_LEVEL_DELAY) {
                    state->level++;
                    StartNewLevel(state);
                    state->nextLevelDelay = 0;
                }
            }
            
            // Handle ship death and respawn
            if (!state->ship.isAlive) {
                if (state->respawnDelay == 0) {
                    // Just died, decrement lives
                    state->ship.lives--;
                    state->respawnDelay = 2.0f; // 2 second delay before respawn
                } else {
                    // Waiting to respawn
                    state->respawnDelay -= deltaTime;
                    if (state->respawnDelay <= 0) {
                        state->respawnDelay = 0;
                        if (state->ship.lives > 0) {
                            RespawnSpaceship(&state->ship, state->screenWidth / 2, state->screenHeight / 2);
                        } else {
                            GameOver(state);
                        }
                    }
                }
            }
            break;
            
        case GAME_STATE_PAUSED:
            break;
            
        case GAME_STATE_GAME_OVER:
            break;
    }
}

void DrawGame(const GameState* state) {
    switch (state->state) {
        case GAME_STATE_MENU:
            DrawText("ASTEROIDS", state->screenWidth/2 - 100, state->screenHeight/2 - 50, 30, WHITE);
            DrawText("Press SPACE to Start", state->screenWidth/2 - 110, state->screenHeight/2, 20, WHITE);
            DrawText("Press F1 for Help", state->screenWidth/2 - 70, state->screenHeight/2 + 50, 14, WHITE);
            break;
            
        case GAME_STATE_PLAYING:
        case GAME_STATE_PAUSED:
            DrawSpaceship(&state->ship);
            
            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                DrawAsteroid(&state->asteroids[i]);
            }
            
            for (int i = 0; i < MAX_BULLETS; i++) {
                DrawBullet(&state->bullets[i]);
            }
            
            for (int i = 0; i < MAX_UFOS; i++) {
                DrawUFO(&state->ufos[i]);
            }
            
            char scoreText[32];
            snprintf(scoreText, sizeof(scoreText), "Score: %d", state->score);
            DrawText(scoreText, 10, 10, 20, WHITE);
            
            snprintf(scoreText, sizeof(scoreText), "High: %d", state->highScore);
            DrawText(scoreText, 10, 35, 20, WHITE);
            
            snprintf(scoreText, sizeof(scoreText), "Level: %d", state->level);
            DrawText(scoreText, 10, 60, 20, WHITE);
            
            for (int i = 0; i < state->ship.lives; i++) {
                Vector2 p1 = {30.0f + i * 25, 90};
                Vector2 p2 = {20.0f + i * 25, 105};
                Vector2 p3 = {40.0f + i * 25, 105};
                DrawLineV(p1, p2, WHITE);
                DrawLineV(p2, p3, WHITE);
                DrawLineV(p3, p1, WHITE);
            }
            
            if (state->state == GAME_STATE_PAUSED) {
                DrawText("PAUSED", state->screenWidth/2 - 50, state->screenHeight/2, 30, WHITE);
            }
            break;
            
        case GAME_STATE_GAME_OVER:
            DrawText("GAME OVER", state->screenWidth/2 - 80, state->screenHeight/2 - 50, 30, WHITE);
            
            char finalScore[64];
            snprintf(finalScore, sizeof(finalScore), "Final Score: %d", state->score);
            DrawText(finalScore, state->screenWidth/2 - 80, state->screenHeight/2, 20, WHITE);
            
            if (state->showingHighScore) {
                DrawText("NEW HIGH SCORE!", state->screenWidth/2 - 90, state->screenHeight/2 + 30, 20, YELLOW);
            }
            
            DrawText("Press SPACE to Play Again", state->screenWidth/2 - 130, state->screenHeight/2 + 60, 20, WHITE);
            break;
    }
}

void FireBullet(GameState* state) {
    if (state->fireDelay > 0 || !state->ship.isAlive) return;
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!state->bullets[i].isActive) {
            InitBullet(&state->bullets[i], state->ship.position, state->ship.rotation, true);
            state->fireDelay = FIRE_DELAY;
            PlayShootSound();
            break;
        }
    }
}

void CheckCollisions(GameState* state) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!state->bullets[i].isActive) continue;
        
        for (int j = 0; j < MAX_ASTEROIDS; j++) {
            if (!state->asteroids[j].isActive) continue;
            
            if (CheckCollisionCircles(state->bullets[i].position, BULLET_RADIUS,
                                     state->asteroids[j].position, state->asteroids[j].radius)) {
                
                if (state->bullets[i].fromPlayer) {
                    UpdateScore(state, GetAsteroidPoints(state->asteroids[j].size));
                }
                
                if (state->asteroids[j].size != ASTEROID_SMALL) {
                    for (int k = 0; k < MAX_ASTEROIDS; k++) {
                        if (!state->asteroids[k].isActive) {
                            for (int l = k + 1; l < MAX_ASTEROIDS; l++) {
                                if (!state->asteroids[l].isActive) {
                                    SplitAsteroid(&state->asteroids[j], 
                                                &state->asteroids[k], 
                                                &state->asteroids[l]);
                                    state->asteroidCount += 2;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                
                DestroyAsteroid(&state->asteroids[j]);
                DestroyBullet(&state->bullets[i]);
                PlayExplosionSound();
                state->asteroidCount--;
                break;
            }
        }
        
        for (int j = 0; j < MAX_UFOS; j++) {
            if (!state->ufos[j].isActive) continue;
            
            if (state->bullets[i].fromPlayer &&
                CheckCollisionCircles(state->bullets[i].position, BULLET_RADIUS,
                                     state->ufos[j].position, UFO_SIZE)) {
                
                UpdateScore(state, GetUFOPoints(state->ufos[j].type));
                DestroyUFO(&state->ufos[j]);
                DestroyBullet(&state->bullets[i]);
                PlayExplosionSound();
                break;
            }
        }
        
        if (!state->bullets[i].fromPlayer && state->ship.isAlive && !IsSpaceshipInvulnerable(&state->ship)) {
            if (CheckCollisionCircles(state->bullets[i].position, BULLET_RADIUS,
                                     state->ship.position, SPACESHIP_SIZE)) {
                state->ship.isAlive = false;
                DestroyBullet(&state->bullets[i]);
                PlayExplosionSound();
            }
        }
    }
    
    if (state->ship.isAlive && !IsSpaceshipInvulnerable(&state->ship)) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (!state->asteroids[i].isActive) continue;
            
            if (CheckCollisionCircles(state->ship.position, SPACESHIP_SIZE,
                                     state->asteroids[i].position, state->asteroids[i].radius)) {
                state->ship.isAlive = false;
                PlayExplosionSound();
                break;
            }
        }
        
        for (int i = 0; i < MAX_UFOS; i++) {
            if (!state->ufos[i].isActive) continue;
            
            if (CheckCollisionCircles(state->ship.position, SPACESHIP_SIZE,
                                     state->ufos[i].position, UFO_SIZE)) {
                state->ship.isAlive = false;
                DestroyUFO(&state->ufos[i]);
                PlayExplosionSound();
                break;
            }
        }
    }
}

void UpdateScore(GameState* state, int points) {
    state->score += points;
    if (state->score > state->highScore) {
        state->highScore = state->score;
        state->showingHighScore = true;
    }
}

void PauseGame(GameState* state) {
    if (state->state == GAME_STATE_PLAYING) {
        state->state = GAME_STATE_PAUSED;
    }
}

void ResumeGame(GameState* state) {
    if (state->state == GAME_STATE_PAUSED) {
        state->state = GAME_STATE_PLAYING;
    }
}

void GameOver(GameState* state) {
    state->state = GAME_STATE_GAME_OVER;
}

int GetAsteroidPoints(AsteroidSize size) {
    switch (size) {
        case ASTEROID_LARGE: return 20;
        case ASTEROID_MEDIUM: return 50;
        case ASTEROID_SMALL: return 100;
        default: return 0;
    }
}

int GetUFOPoints(UFOType type) {
    return (type == UFO_LARGE) ? 200 : 1000;
}