#ifndef GAME_H
#define GAME_H

#include "entities.h"
#include <stdbool.h>

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER
} GameStateType;

typedef struct {
    Spaceship ship;
    Asteroid asteroids[MAX_ASTEROIDS];
    Bullet bullets[MAX_BULLETS];
    UFO ufos[MAX_UFOS];
    
    GameStateType state;
    int score;
    int highScore;
    int level;
    int asteroidCount;
    
    float screenWidth;
    float screenHeight;
    
    float ufoSpawnTimer;
    float nextUFOSpawn;
    
    float fireDelay;
    float nextLevelDelay;
    float respawnDelay;
    
    bool showingHighScore;
} GameState;

GameState* CreateGameState(float screenWidth, float screenHeight);
void DestroyGameState(GameState* state);

void InitGame(GameState* state);
void UpdateGame(GameState* state, float deltaTime);
void DrawGame(const GameState* state);

void StartNewGame(GameState* state);
void StartNewLevel(GameState* state);
void SpawnAsteroids(GameState* state, int count);
void SpawnUFO(GameState* state);

void FireBullet(GameState* state);
void CheckCollisions(GameState* state);
void UpdateScore(GameState* state, int points);

void PauseGame(GameState* state);
void ResumeGame(GameState* state);
void GameOver(GameState* state);

int GetAsteroidPoints(AsteroidSize size);
int GetUFOPoints(UFOType type);

#endif