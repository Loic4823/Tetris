#include <stdio.h>
#include <SDL_mixer.h>
#include "audio.h"

// Variable globale (singleton) pour l'état audio.
// Accessible partout via 'extern' dans audio.h.
AudioState audioSystem;

// Initialise les variables de volume au démarrage.
// Ne lance pas encore la musique, configure juste l'état interne.
void InitAudio() {
    audioSystem.volume = 0.0f;          // Commence silencieux (choix de design).
    audioSystem.previousVolume = 0.5f;  // Mémoire pour le "Unmute".
    audioSystem.isMuted = true;

    ApplyVolume(); // Applique immédiatement cet état au mixer.
}

// [IMPORTANT] Fonction de synchronisation Logique -> SDL_Mixer.
// Doit être appelée à chaque fois que 'audioSystem.volume' change.
void ApplyVolume() {
    // 1. Conversion du volume :
    // SDL_Mixer gère le volume de 0 à MIX_MAX_VOLUME (128).
    // On convertit notre float (0.0 - 1.0) vers cet intervalle.
    int vol = (int)(audioSystem.volume * 128);
    
    // 2. Priorité au Mute :
    // Si le drapeau muet est actif, on force le volume à 0, peu importe le réglage.
    if (audioSystem.isMuted) {
        vol = 0;
    }

    // 3. Application à la musique (Stream) :
    // La musique est streamée (lue au fur et à mesure du disque).
    Mix_VolumeMusic(vol);

    // 4. Application aux bruitages (Chunks) :
    // Mix_Volume gère les canaux d'effets sonores.
    // L'argument "-1" est un joker qui signifie "Appliquer à TOUS les canaux actifs".
    Mix_Volume(-1, vol); 
}