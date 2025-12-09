#include <stdio.h>
#include <SDL_mixer.h>
#include "audio.h"

AudioState audioSystem;

void InitAudio() {
    audioSystem.volume = 0.0f;
    audioSystem.previousVolume = 0.5f;
    audioSystem.isMuted = true;
    ApplyVolume();
}

void ApplyVolume() {
    int vol = (int)(audioSystem.volume * 128);
    
    if (audioSystem.isMuted) {
        vol = 0;
    }

    Mix_VolumeMusic(vol);
    Mix_Volume(-1, vol); 
}