#ifndef AUDIO_H
#define AUDIO_H

void InitGameAudio(void);
void CloseGameAudio(void);

void PlayShootSound(void);
void PlayExplosionSound(void);
void PlayThrustSound(void);
void StopThrustSound(void);
void PlayHyperspaceSound(void);
void PlayUFOSound(void);
void StopUFOSound(void);

#endif