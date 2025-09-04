#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    ASTEROID_LARGE,
    ASTEROID_MEDIUM,
    ASTEROID_SMALL
} AsteroidSize;

typedef enum {
    UFO_LARGE,
    UFO_SMALL
} UFOType;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float rotationSpeed;
    bool isThrusting;
    bool isAlive;
    float invulnerableTime;
    int lives;
} Spaceship;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float rotationSpeed;
    AsteroidSize size;
    bool isActive;
    float radius;
    Vector2* shape;
    int shapePointCount;
} Asteroid;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    bool isActive;
    bool fromPlayer;
} Bullet;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    UFOType type;
    bool isActive;
    float shootTimer;
    float moveTimer;
    int direction;
} UFO;

#define MAX_ASTEROIDS 28
#define MAX_BULLETS 32
#define MAX_UFOS 2
#define MAX_ASTEROID_VERTICES 12

#define SPACESHIP_SIZE 10.0f
#define SPACESHIP_THRUST_POWER 250.0f
#define SPACESHIP_ROTATION_SPEED 250.0f
#define SPACESHIP_MAX_SPEED 400.0f
#define SPACESHIP_DRAG 0.99f
#define SPACESHIP_INVULNERABLE_TIME 3.0f

#define BULLET_SPEED 500.0f
#define BULLET_LIFETIME 1.2f
#define BULLET_RADIUS 2.0f

#define ASTEROID_LARGE_RADIUS 40.0f
#define ASTEROID_MEDIUM_RADIUS 25.0f
#define ASTEROID_SMALL_RADIUS 15.0f
#define ASTEROID_SPEED_MIN 20.0f
#define ASTEROID_SPEED_MAX 100.0f

#define UFO_LARGE_SPEED 100.0f
#define UFO_SMALL_SPEED 150.0f
#define UFO_SHOOT_INTERVAL 1.5f
#define UFO_SIZE 20.0f

void InitSpaceship(Spaceship* ship, float x, float y);
void RespawnSpaceship(Spaceship* ship, float x, float y);
void UpdateSpaceship(Spaceship* ship, float deltaTime);
void DrawSpaceship(const Spaceship* ship);
void ThrustSpaceship(Spaceship* ship);
void RotateSpaceship(Spaceship* ship, float direction);
void HyperspaceJump(Spaceship* ship, float screenWidth, float screenHeight);
bool IsSpaceshipInvulnerable(const Spaceship* ship);

void InitAsteroid(Asteroid* asteroid, float x, float y, AsteroidSize size);
void UpdateAsteroid(Asteroid* asteroid, float deltaTime, float screenWidth, float screenHeight);
void DrawAsteroid(const Asteroid* asteroid);
void SplitAsteroid(const Asteroid* parent, Asteroid* child1, Asteroid* child2);
void DestroyAsteroid(Asteroid* asteroid);

void InitBullet(Bullet* bullet, Vector2 position, float angle, bool fromPlayer);
void UpdateBullet(Bullet* bullet, float deltaTime, float screenWidth, float screenHeight);
void DrawBullet(const Bullet* bullet);
void DestroyBullet(Bullet* bullet);

void InitUFO(UFO* ufo, UFOType type, float screenWidth, float screenHeight);
void UpdateUFO(UFO* ufo, float deltaTime, const Spaceship* target, Bullet* bullets, int maxBullets, float screenWidth);
void DrawUFO(const UFO* ufo);
void DestroyUFO(UFO* ufo);

void WrapPosition(Vector2* position, float screenWidth, float screenHeight);

#endif