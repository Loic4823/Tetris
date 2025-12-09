#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

typedef struct {
    float volume;
    float previousVolume;
    bool isMuted;
} AudioState;

extern AudioState audioSystem;

void InitAudio();
void ApplyVolume();

#endif