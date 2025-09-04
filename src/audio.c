#include "audio.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
    Sound shoot;
    Sound explosion;
    Sound thrust;
    Sound hyperspace;
    Sound ufo;
    bool thrustPlaying;
    bool ufoPlaying;
} GameSounds;

static GameSounds sounds = {0};

static Sound CreateSyntheticSound(float frequency, float duration, int sampleRate) {
    int sampleCount = (int)(duration * sampleRate);
    float* data = calloc(sampleCount, sizeof(float));
    
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - (t / duration);
        data[i] = sinf(2.0f * PI * frequency * t) * envelope * 0.3f;
        
        if (frequency > 200) {
            data[i] += sinf(2.0f * PI * frequency * 0.5f * t) * envelope * 0.1f;
        }
    }
    
    Wave wave = {
        .frameCount = sampleCount,
        .sampleRate = sampleRate,
        .sampleSize = 32,
        .channels = 1,
        .data = data
    };
    
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    
    return sound;
}

static Sound CreateNoiseSound(float duration, int sampleRate) {
    int sampleCount = (int)(duration * sampleRate);
    float* data = calloc(sampleCount, sizeof(float));
    
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - (t / duration);
        data[i] = ((float)GetRandomValue(-100, 100) / 100.0f) * envelope * 0.3f;
    }
    
    Wave wave = {
        .frameCount = sampleCount,
        .sampleRate = sampleRate,
        .sampleSize = 32,
        .channels = 1,
        .data = data
    };
    
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    
    return sound;
}

void InitGameAudio(void) {
    sounds.shoot = CreateSyntheticSound(500.0f, 0.1f, 44100);
    sounds.explosion = CreateNoiseSound(0.3f, 44100);
    sounds.thrust = CreateSyntheticSound(100.0f, 0.5f, 44100);
    sounds.hyperspace = CreateSyntheticSound(800.0f, 0.2f, 44100);
    sounds.ufo = CreateSyntheticSound(150.0f, 2.0f, 44100);
    
    SetSoundVolume(sounds.shoot, 0.5f);
    SetSoundVolume(sounds.explosion, 0.6f);
    SetSoundVolume(sounds.thrust, 0.3f);
    SetSoundVolume(sounds.hyperspace, 0.4f);
    SetSoundVolume(sounds.ufo, 0.3f);
}

void CloseGameAudio(void) {
    UnloadSound(sounds.shoot);
    UnloadSound(sounds.explosion);
    UnloadSound(sounds.thrust);
    UnloadSound(sounds.hyperspace);
    UnloadSound(sounds.ufo);
}

void PlayShootSound(void) {
    PlaySound(sounds.shoot);
}

void PlayExplosionSound(void) {
    PlaySound(sounds.explosion);
}

void PlayThrustSound(void) {
    if (!sounds.thrustPlaying) {
        PlaySound(sounds.thrust);
        sounds.thrustPlaying = true;
    }
}

void StopThrustSound(void) {
    if (sounds.thrustPlaying) {
        StopSound(sounds.thrust);
        sounds.thrustPlaying = false;
    }
}

void PlayHyperspaceSound(void) {
    PlaySound(sounds.hyperspace);
}

void PlayUFOSound(void) {
    if (!sounds.ufoPlaying) {
        PlaySound(sounds.ufo);
        sounds.ufoPlaying = true;
    }
}

void StopUFOSound(void) {
    if (sounds.ufoPlaying) {
        StopSound(sounds.ufo);
        sounds.ufoPlaying = false;
    }
}