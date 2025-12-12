#include <stdio.h>
#include <SDL_mixer.h>
#include "audio.h"

// Déclaration de notre "tableau de bord" audio. 
// C'est une variable globale accessible partout pour savoir si le son est activé ou quel est le volume.
AudioState audioSystem;

// Cette fonction est appelée une seule fois, au lancement du jeu, pour préparer le terrain.
void InitAudio() {
    // On commence avec le volume à 0 (silence) pour ne pas surprendre le joueur au lancement.
    audioSystem.volume = 0.0f;

    // On garde en mémoire un volume "par défaut" (50%) pour le moment où le joueur réactivera le son.
    audioSystem.previousVolume = 0.5f;

    // On active le mode "Muet" dès le départ. Le jeu se lancera donc en silence total.
    audioSystem.isMuted = true;

    // Maintenant que les variables sont prêtes, on applique ces réglages au moteur audio SDL.
    ApplyVolume();
}

// C'est la fonction "chef d'orchestre". C'est elle qui parle directement à la carte son via SDL.
void ApplyVolume() {
    // SDL ne comprend pas les volumes de 0.0 à 1.0 (pourcentages).
    // Il veut un chiffre entre 0 et 128. Donc on fait une règle de trois pour convertir.
    // (int) sert à arrondir le résultat pour avoir un nombre entier.
    int vol = (int)(audioSystem.volume * 128);
    
    // Ici, c'est la sécurité "Couper le son".
    // Si le drapeau "isMuted" est levé, on ignore le volume calculé au-dessus 
    // et on force le volume à 0 absolu.
    if (audioSystem.isMuted) {
        vol = 0;
    }

    // 1. On applique le volume à la musique de fond (le fichier MP3 long).
    Mix_VolumeMusic(vol);

    // 2. On applique le volume aux bruitages (les effets sonores courts comme 'clear.wav').
    // L'argument "-1" est une astuce magique de SDL qui veut dire : 
    // "Applique ce volume à TOUTES les pistes d'effets sonores en même temps".
    Mix_Volume(-1, vol); 
}