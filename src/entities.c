#include "entities.h"
#include "utils.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void InitSpaceship(Spaceship* ship, float x, float y) {
    ship->position = (Vector2){x, y};
    ship->velocity = (Vector2){0, 0};
    ship->rotation = 0;
    ship->rotationSpeed = 0;
    ship->isThrusting = false;
    ship->isAlive = true;
    ship->invulnerableTime = SPACESHIP_INVULNERABLE_TIME;
    ship->lives = 3;
}

void RespawnSpaceship(Spaceship* ship, float x, float y) {
    ship->position = (Vector2){x, y};
    ship->velocity = (Vector2){0, 0};
    ship->rotation = 0;
    ship->rotationSpeed = 0;
    ship->isThrusting = false;
    ship->isAlive = true;
    ship->invulnerableTime = SPACESHIP_INVULNERABLE_TIME;
    // Don't reset lives - keep the current value
}

void UpdateSpaceship(Spaceship* ship, float deltaTime) {
    if (!ship->isAlive) return;
    
    ship->rotation += ship->rotationSpeed * deltaTime;
    
    if (ship->isThrusting) {
        float radians = (ship->rotation - 90) * DEG2RAD;
        ship->velocity.x += cos(radians) * SPACESHIP_THRUST_POWER * deltaTime;
        ship->velocity.y += sin(radians) * SPACESHIP_THRUST_POWER * deltaTime;
        
        float speed = Vector2Length(ship->velocity);
        if (speed > SPACESHIP_MAX_SPEED) {
            ship->velocity = Vector2Scale(Vector2Normalize(ship->velocity), SPACESHIP_MAX_SPEED);
        }
    }
    
    ship->velocity = Vector2Scale(ship->velocity, SPACESHIP_DRAG);
    
    ship->position.x += ship->velocity.x * deltaTime;
    ship->position.y += ship->velocity.y * deltaTime;
    
    if (ship->invulnerableTime > 0) {
        ship->invulnerableTime -= deltaTime;
    }
}

void DrawSpaceship(const Spaceship* ship) {
    if (!ship->isAlive) return;
    
    if (IsSpaceshipInvulnerable(ship)) {
        if ((int)(ship->invulnerableTime * 10) % 2 == 0) return;
    }
    
    Vector2 v1 = {0, -SPACESHIP_SIZE};
    Vector2 v2 = {-SPACESHIP_SIZE * 0.7f, SPACESHIP_SIZE};
    Vector2 v3 = {SPACESHIP_SIZE * 0.7f, SPACESHIP_SIZE};
    
    float radians = ship->rotation * DEG2RAD;
    float cosR = cos(radians);
    float sinR = sin(radians);
    
    Vector2 tv1 = {
        v1.x * cosR - v1.y * sinR + ship->position.x,
        v1.x * sinR + v1.y * cosR + ship->position.y
    };
    Vector2 tv2 = {
        v2.x * cosR - v2.y * sinR + ship->position.x,
        v2.x * sinR + v2.y * cosR + ship->position.y
    };
    Vector2 tv3 = {
        v3.x * cosR - v3.y * sinR + ship->position.x,
        v3.x * sinR + v3.y * cosR + ship->position.y
    };
    
    DrawLineV(tv1, tv2, WHITE);
    DrawLineV(tv2, tv3, WHITE);
    DrawLineV(tv3, tv1, WHITE);
    
    if (ship->isThrusting) {
        Vector2 thrust1 = {-SPACESHIP_SIZE * 0.4f, SPACESHIP_SIZE};
        Vector2 thrust2 = {0, SPACESHIP_SIZE * 1.5f};
        Vector2 thrust3 = {SPACESHIP_SIZE * 0.4f, SPACESHIP_SIZE};
        
        Vector2 tt1 = {
            thrust1.x * cosR - thrust1.y * sinR + ship->position.x,
            thrust1.x * sinR + thrust1.y * cosR + ship->position.y
        };
        Vector2 tt2 = {
            thrust2.x * cosR - thrust2.y * sinR + ship->position.x,
            thrust2.x * sinR + thrust2.y * cosR + ship->position.y
        };
        Vector2 tt3 = {
            thrust3.x * cosR - thrust3.y * sinR + ship->position.x,
            thrust3.x * sinR + thrust3.y * cosR + ship->position.y
        };
        
        DrawLineV(tt1, tt2, ORANGE);
        DrawLineV(tt2, tt3, ORANGE);
    }
}

void ThrustSpaceship(Spaceship* ship) {
    ship->isThrusting = true;
}

void RotateSpaceship(Spaceship* ship, float direction) {
    ship->rotationSpeed = direction * SPACESHIP_ROTATION_SPEED;
}

void HyperspaceJump(Spaceship* ship, float screenWidth, float screenHeight) {
    ship->position.x = RandomFloat(0, screenWidth);
    ship->position.y = RandomFloat(0, screenHeight);
    ship->velocity = (Vector2){0, 0};
}

bool IsSpaceshipInvulnerable(const Spaceship* ship) {
    return ship->invulnerableTime > 0;
}

void InitAsteroid(Asteroid* asteroid, float x, float y, AsteroidSize size) {
    asteroid->position = (Vector2){x, y};
    asteroid->size = size;
    asteroid->isActive = true;
    
    switch(size) {
        case ASTEROID_LARGE:
            asteroid->radius = ASTEROID_LARGE_RADIUS;
            break;
        case ASTEROID_MEDIUM:
            asteroid->radius = ASTEROID_MEDIUM_RADIUS;
            break;
        case ASTEROID_SMALL:
            asteroid->radius = ASTEROID_SMALL_RADIUS;
            break;
    }
    
    float speed = RandomFloat(ASTEROID_SPEED_MIN, ASTEROID_SPEED_MAX);
    float angle = RandomFloat(0, 360) * DEG2RAD;
    asteroid->velocity = (Vector2){cos(angle) * speed, sin(angle) * speed};
    
    asteroid->rotation = RandomFloat(0, 360);
    asteroid->rotationSpeed = RandomFloat(-100, 100);
    
    asteroid->shapePointCount = RandomInt(8, MAX_ASTEROID_VERTICES);
    asteroid->shape = malloc(sizeof(Vector2) * asteroid->shapePointCount);
    
    for (int i = 0; i < asteroid->shapePointCount; i++) {
        float angleStep = (360.0f / asteroid->shapePointCount) * i * DEG2RAD;
        float radiusVariation = RandomFloat(0.8f, 1.2f);
        asteroid->shape[i] = (Vector2){
            cos(angleStep) * asteroid->radius * radiusVariation,
            sin(angleStep) * asteroid->radius * radiusVariation
        };
    }
}

void UpdateAsteroid(Asteroid* asteroid, float deltaTime, float screenWidth, float screenHeight) {
    if (!asteroid->isActive) return;
    
    asteroid->position.x += asteroid->velocity.x * deltaTime;
    asteroid->position.y += asteroid->velocity.y * deltaTime;
    asteroid->rotation += asteroid->rotationSpeed * deltaTime;
    
    WrapPosition(&asteroid->position, screenWidth, screenHeight);
}

void DrawAsteroid(const Asteroid* asteroid) {
    if (!asteroid->isActive || !asteroid->shape) return;
    
    float radians = asteroid->rotation * DEG2RAD;
    float cosR = cos(radians);
    float sinR = sin(radians);
    
    for (int i = 0; i < asteroid->shapePointCount; i++) {
        int next = (i + 1) % asteroid->shapePointCount;
        
        Vector2 p1 = asteroid->shape[i];
        Vector2 p2 = asteroid->shape[next];
        
        Vector2 tp1 = {
            p1.x * cosR - p1.y * sinR + asteroid->position.x,
            p1.x * sinR + p1.y * cosR + asteroid->position.y
        };
        Vector2 tp2 = {
            p2.x * cosR - p2.y * sinR + asteroid->position.x,
            p2.x * sinR + p2.y * cosR + asteroid->position.y
        };
        
        DrawLineV(tp1, tp2, WHITE);
    }
}

void SplitAsteroid(const Asteroid* parent, Asteroid* child1, Asteroid* child2) {
    if (parent->size == ASTEROID_SMALL) return;
    
    AsteroidSize newSize = (parent->size == ASTEROID_LARGE) ? ASTEROID_MEDIUM : ASTEROID_SMALL;
    
    InitAsteroid(child1, parent->position.x, parent->position.y, newSize);
    InitAsteroid(child2, parent->position.x, parent->position.y, newSize);
    
    float angle1 = RandomFloat(0, 360) * DEG2RAD;
    float angle2 = angle1 + PI;
    float speed = RandomFloat(ASTEROID_SPEED_MIN * 1.5f, ASTEROID_SPEED_MAX * 1.5f);
    
    child1->velocity = (Vector2){cos(angle1) * speed, sin(angle1) * speed};
    child2->velocity = (Vector2){cos(angle2) * speed, sin(angle2) * speed};
}

void DestroyAsteroid(Asteroid* asteroid) {
    asteroid->isActive = false;
    if (asteroid->shape) {
        free(asteroid->shape);
        asteroid->shape = nullptr;
    }
}

void InitBullet(Bullet* bullet, Vector2 position, float angle, bool fromPlayer) {
    bullet->position = position;
    bullet->lifetime = BULLET_LIFETIME;
    bullet->isActive = true;
    bullet->fromPlayer = fromPlayer;
    
    float radians = (angle - 90) * DEG2RAD;
    bullet->velocity = (Vector2){
        cos(radians) * BULLET_SPEED,
        sin(radians) * BULLET_SPEED
    };
}

void UpdateBullet(Bullet* bullet, float deltaTime, float screenWidth, float screenHeight) {
    if (!bullet->isActive) return;
    
    bullet->position.x += bullet->velocity.x * deltaTime;
    bullet->position.y += bullet->velocity.y * deltaTime;
    
    bullet->lifetime -= deltaTime;
    if (bullet->lifetime <= 0) {
        bullet->isActive = false;
    }
    
    WrapPosition(&bullet->position, screenWidth, screenHeight);
}

void DrawBullet(const Bullet* bullet) {
    if (!bullet->isActive) return;
    DrawCircleV(bullet->position, BULLET_RADIUS, WHITE);
}

void DestroyBullet(Bullet* bullet) {
    bullet->isActive = false;
}

void InitUFO(UFO* ufo, UFOType type, float screenWidth, float screenHeight) {
    ufo->type = type;
    ufo->isActive = true;
    ufo->shootTimer = 0;
    ufo->moveTimer = 0;
    
    if (RandomInt(0, 1) == 0) {
        ufo->position.x = 0;
        ufo->direction = 1;
    } else {
        ufo->position.x = screenWidth;
        ufo->direction = -1;
    }
    
    ufo->position.y = RandomFloat(screenHeight * 0.2f, screenHeight * 0.8f);
    
    float speed = (type == UFO_LARGE) ? UFO_LARGE_SPEED : UFO_SMALL_SPEED;
    ufo->velocity = (Vector2){speed * ufo->direction, 0};
}

void UpdateUFO(UFO* ufo, float deltaTime, const Spaceship* target, Bullet* bullets, int maxBullets, float screenWidth) {
    if (!ufo->isActive) return;
    
    ufo->position.x += ufo->velocity.x * deltaTime;
    
    if (ufo->type == UFO_SMALL) {
        ufo->moveTimer += deltaTime;
        if (ufo->moveTimer > 0.5f) {
            ufo->velocity.y = RandomFloat(-50, 50);
            ufo->moveTimer = 0;
        }
        ufo->position.y += ufo->velocity.y * deltaTime;
    }
    
    if ((ufo->direction > 0 && ufo->position.x > screenWidth + UFO_SIZE) ||
        (ufo->direction < 0 && ufo->position.x < -UFO_SIZE)) {
        ufo->isActive = false;
    }
    
    ufo->shootTimer += deltaTime;
    if (ufo->shootTimer > UFO_SHOOT_INTERVAL) {
        ufo->shootTimer = 0;
        
        for (int i = 0; i < maxBullets; i++) {
            if (!bullets[i].isActive) {
                float angle;
                if (ufo->type == UFO_SMALL && target && target->isAlive) {
                    Vector2 toPlayer = Vector2Subtract(target->position, ufo->position);
                    angle = atan2f(toPlayer.y, toPlayer.x) * RAD2DEG + 90;
                } else {
                    angle = RandomFloat(0, 360);
                }
                
                InitBullet(&bullets[i], ufo->position, angle, false);
                break;
            }
        }
    }
}

void DrawUFO(const UFO* ufo) {
    if (!ufo->isActive) return;
    
    float size = (ufo->type == UFO_LARGE) ? UFO_SIZE : UFO_SIZE * 0.6f;
    
    DrawLineV(
        (Vector2){ufo->position.x - size, ufo->position.y},
        (Vector2){ufo->position.x + size, ufo->position.y},
        WHITE
    );
    
    DrawLineV(
        (Vector2){ufo->position.x - size * 0.5f, ufo->position.y - size * 0.3f},
        (Vector2){ufo->position.x + size * 0.5f, ufo->position.y - size * 0.3f},
        WHITE
    );
    
    DrawLineV(
        (Vector2){ufo->position.x - size * 0.5f, ufo->position.y - size * 0.3f},
        (Vector2){ufo->position.x - size, ufo->position.y},
        WHITE
    );
    
    DrawLineV(
        (Vector2){ufo->position.x + size * 0.5f, ufo->position.y - size * 0.3f},
        (Vector2){ufo->position.x + size, ufo->position.y},
        WHITE
    );
    
    DrawLineV(
        (Vector2){ufo->position.x - size * 0.5f, ufo->position.y + size * 0.3f},
        (Vector2){ufo->position.x + size * 0.5f, ufo->position.y + size * 0.3f},
        WHITE
    );
    
    DrawLineV(
        (Vector2){ufo->position.x - size * 0.5f, ufo->position.y + size * 0.3f},
        (Vector2){ufo->position.x - size, ufo->position.y},
        WHITE
    );
    
    DrawLineV(
        (Vector2){ufo->position.x + size * 0.5f, ufo->position.y + size * 0.3f},
        (Vector2){ufo->position.x + size, ufo->position.y},
        WHITE
    );
}

void DestroyUFO(UFO* ufo) {
    ufo->isActive = false;
}

void WrapPosition(Vector2* position, float screenWidth, float screenHeight) {
    if (position->x < 0) position->x = screenWidth;
    if (position->x > screenWidth) position->x = 0;
    if (position->y < 0) position->y = screenHeight;
    if (position->y > screenHeight) position->y = 0;
}