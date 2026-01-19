#include "input.h"
#include <SDL.h>

// Tableau de correspondance (Mapping).
// L'index du tableau correspond à l'enum GameAction (0 = UP, 1 = DOWN, etc.).
KeyBinding bindings[ACTION_COUNT];

// Configure les touches par défaut au premier lancement (Hardcodé).
void InitInputProfile() {
    // [STRATÉGIE] Double Mapping :
    // On offre une touche "Main Gauche" (ZQSD) et une "Main Droite" (Flèches) par défaut.
    
    bindings[ACTION_UP].keyPrimary = SDLK_z;       
    bindings[ACTION_UP].keySecondary = SDLK_UP;    

    bindings[ACTION_DOWN].keyPrimary = SDLK_s;     
    bindings[ACTION_DOWN].keySecondary = SDLK_DOWN;

    bindings[ACTION_LEFT].keyPrimary = SDLK_q;     
    bindings[ACTION_LEFT].keySecondary = SDLK_LEFT;

    bindings[ACTION_RIGHT].keyPrimary = SDLK_d;    
    bindings[ACTION_RIGHT].keySecondary = SDLK_RIGHT;

    // Actions de jeu
    bindings[ACTION_A].keyPrimary = SDLK_a;        // Rotation
    bindings[ACTION_A].keySecondary = 0;           // Pas de secondaire

    bindings[ACTION_E].keyPrimary = SDLK_e;        
    bindings[ACTION_E].keySecondary = 0;

    bindings[ACTION_C].keyPrimary = SDLK_c;        // Hold
    bindings[ACTION_C].keySecondary = 0;

    // Actions Système (ne font pas bouger la pièce)
    bindings[ACTION_MUTE].keyPrimary = SDLK_m;     
    bindings[ACTION_MUTE].keySecondary = 0;

    bindings[ACTION_VOL_UP].keyPrimary = SDLK_r;       
    bindings[ACTION_VOL_UP].keySecondary = SDLK_KP_PLUS; 

    bindings[ACTION_VOL_DOWN].keyPrimary = SDLK_f;       
    bindings[ACTION_VOL_DOWN].keySecondary = SDLK_KP_MINUS; 
}

// [CŒUR DU SYSTÈME]
// Vérifie si la touche physique appuyée (keyPressed) correspond à l'action demandée.
bool IsActionPressed(GameAction action, int keyPressed) {
    // Vérifie la touche principale...
    if (keyPressed == bindings[action].keyPrimary) return true;
    // ...OU la touche secondaire si elle existe.
    if (bindings[action].keySecondary != 0 && keyPressed == bindings[action].keySecondary) return true;
    
    return false;
}

// Met à jour une liaison (utilisé par le menu de configuration).
void UpdateKeyBinding(GameAction action, SDL_Keycode newKey, bool isSecondary) {
    if (isSecondary) {
        bindings[action].keySecondary = newKey; 
    } else {
        bindings[action].keyPrimary = newKey;   
    }
}

// Helper pour l'interface graphique : permet d'afficher le nom de la touche configurée.
SDL_Keycode GetKeyBinding(GameAction action, bool secondary) {
    if (secondary) return bindings[action].keySecondary;
    return bindings[action].keyPrimary;
}

// [PERSISTANCE] 
// Sauvegarde binaire brute de la structure (très rapide et simple).
void SaveInputProfile(FILE* f) {
    fwrite(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
}

void LoadInputProfile(FILE* f) {
    fread(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
}