#include "utils.h"
#include <stdlib.h>
#include <time.h>

static bool randomInitialized = false;

static void EnsureRandomInit(void) {
    if (!randomInitialized) {
        srand((unsigned int)time(nullptr));
        randomInitialized = true;
    }
}

float RandomFloat(float min, float max) {
    EnsureRandomInit();
    float scale = rand() / (float)RAND_MAX;
    return min + scale * (max - min);
}

int RandomInt(int min, int max) {
    EnsureRandomInit();
    return min + rand() % (max - min + 1);
}

