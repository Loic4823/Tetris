#include "input.h" // Nos définitions.
#include <SDL.h>   // SDL.

KeyBinding bindings[ACTION_COUNT]; // Tableau qui stocke la config de toutes les touches.

// On définit les touches par défaut (AZERTY + Flèches).
void InitInputProfile() {
    
    // Mouvements
    bindings[ACTION_UP].keyPrimary = SDLK_z;       // Z pour monter.
    bindings[ACTION_UP].keySecondary = SDLK_UP;    // Flèche Haut.

    bindings[ACTION_DOWN].keyPrimary = SDLK_s;     // S pour descendre.
    bindings[ACTION_DOWN].keySecondary = SDLK_DOWN;// Flèche Bas.

    bindings[ACTION_LEFT].keyPrimary = SDLK_q;     // Q pour gauche (AZERTY).
    bindings[ACTION_LEFT].keySecondary = SDLK_LEFT;// Flèche Gauche.

    bindings[ACTION_RIGHT].keyPrimary = SDLK_d;    // D pour droite.
    bindings[ACTION_RIGHT].keySecondary = SDLK_RIGHT;// Flèche Droite.

    // Actions
    bindings[ACTION_A].keyPrimary = SDLK_a;        // A pour tourner.
    bindings[ACTION_A].keySecondary = 0;           // Pas de 2ème touche.

    bindings[ACTION_E].keyPrimary = SDLK_e;        // E pour tourner l'autre sens.
    bindings[ACTION_E].keySecondary = 0;

    bindings[ACTION_C].keyPrimary = SDLK_c;        // C pour Hold.
    bindings[ACTION_C].keySecondary = 0;

    // Système (Son)
    bindings[ACTION_MUTE].keyPrimary = SDLK_m;     // M pour Muet.
    bindings[ACTION_MUTE].keySecondary = 0;

    bindings[ACTION_VOL_UP].keyPrimary = SDLK_r;       // R pour monter son.
    bindings[ACTION_VOL_UP].keySecondary = SDLK_KP_PLUS; // "+" pavé num.

    bindings[ACTION_VOL_DOWN].keyPrimary = SDLK_f;       // F pour baisser son.
    bindings[ACTION_VOL_DOWN].keySecondary = SDLK_KP_MINUS; // "-" pavé num.
}

// Fonction qui check si la touche pressée (keyPressed) correspond à l'action demandée.
bool IsActionPressed(GameAction action, int keyPressed) {
    if (keyPressed == bindings[action].keyPrimary) return true; // C'est la touche 1 ? Oui.
    if (bindings[action].keySecondary != 0 && keyPressed == bindings[action].keySecondary) return true; // Touche 2 ? Oui.
    return false; // Non, ça ne correspond pas.
}

// Met à jour une touche dans le tableau.
void UpdateKeyBinding(GameAction action, SDL_Keycode newKey, bool isSecondary) {
    if (isSecondary) {
        bindings[action].keySecondary = newKey; // Modifie colonne droite.
    } else {
        bindings[action].keyPrimary = newKey;   // Modifie colonne gauche.
    }
}

// Renvoie le code de la touche (pour l'affichage).
SDL_Keycode GetKeyBinding(GameAction action, bool secondary) {
    if (secondary) return bindings[action].keySecondary;
    return bindings[action].keyPrimary;
}

// Écrit tout le tableau de touches dans le fichier de sauvegarde.
void SaveInputProfile(FILE* f) {
    fwrite(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
}

// Lit le tableau de touches depuis le fichier de sauvegarde.
void LoadInputProfile(FILE* f) {
    fread(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
}