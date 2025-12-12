#ifndef AUDIO_H // Protection contre double inclusion.
#define AUDIO_H

#include <stdbool.h> // Pour utiliser "bool" (vrai/faux).

// Structure pour l'état de l'audio.
typedef struct {
    float volume;         // Volume actuel (0.0 à 1.0).
    float previousVolume; // Volume mémorisé avant de passer en muet.
    bool isMuted;         // Est-ce qu'on est en mode muet ?
} AudioState;

extern AudioState audioSystem; // On dit que cette variable existe ailleurs (dans audio.c).

void InitAudio();   // Fonction pour démarrer le son.
void ApplyVolume(); // Fonction pour mettre à jour le volume SDL.

#endif