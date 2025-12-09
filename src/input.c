#include "input.h" // Inclu les defs de touches et actions
#include <SDL.h>   // Inclu SDL pour les codes clavier

KeyBinding bindings[ACTION_COUNT]; // Tableau global pour stocker la config des touches

// Initialise les touches par défaut (AZERTY + Flèches)
void InitInputProfile() {
    
    // --- DEPLACEMENTS (AZERTY FORCE) ---
    
    bindings[ACTION_UP].keyPrimary = SDLK_z;       // Z pour Haut (Hard Drop)
    bindings[ACTION_UP].keySecondary = SDLK_UP;    // Flèche Haut en secondaire

    bindings[ACTION_DOWN].keyPrimary = SDLK_s;     // S pour Bas (Soft Drop)
    bindings[ACTION_DOWN].keySecondary = SDLK_DOWN;// Flèche Bas en secondaire

    bindings[ACTION_LEFT].keyPrimary = SDLK_q;     // Q pour Gauche (Au lieu de A)
    bindings[ACTION_LEFT].keySecondary = SDLK_LEFT;// Flèche Gauche en secondaire

    bindings[ACTION_RIGHT].keyPrimary = SDLK_d;    // D pour Droite
    bindings[ACTION_RIGHT].keySecondary = SDLK_RIGHT;// Flèche Droite en secondaire

    // --- ACTIONS JEU ---

    bindings[ACTION_A].keyPrimary = SDLK_a;        // A pour tourner (Au lieu de Q)
    bindings[ACTION_A].keySecondary = 0;           // Pas de 2eme touche

    bindings[ACTION_E].keyPrimary = SDLK_e;        // E pour tourner dans l'autre sens
    bindings[ACTION_E].keySecondary = 0;           // Pas de 2eme touche

    bindings[ACTION_C].keyPrimary = SDLK_c;        // C pour stocker la pièce
    bindings[ACTION_C].keySecondary = 0;           // Pas de 2eme touche

    // --- SYSTEME / SON ---

    bindings[ACTION_MUTE].keyPrimary = SDLK_m;     // M pour couper le son
    bindings[ACTION_MUTE].keySecondary = 0;

    bindings[ACTION_VOL_UP].keyPrimary = SDLK_r;       // R pour monter le son
    bindings[ACTION_VOL_UP].keySecondary = SDLK_KP_PLUS; // "+" du pavé numérique

    bindings[ACTION_VOL_DOWN].keyPrimary = SDLK_f;       // F pour baisser le son
    bindings[ACTION_VOL_DOWN].keySecondary = SDLK_KP_MINUS; // "-" du pavé numérique
}

// Vérifie si la touche appuyée correspond à une action
bool IsActionPressed(GameAction action, int keyPressed) {
    if (keyPressed == bindings[action].keyPrimary) return true; // C'est la touche principale ? -> Vrai
    if (bindings[action].keySecondary != 0 && keyPressed == bindings[action].keySecondary) return true; // C'est la touche secondaire ? -> Vrai
    return false; // Sinon -> Faux
}

// Change une touche (utilisé par le menu config)
void UpdateKeyBinding(GameAction action, SDL_Keycode newKey, bool isSecondary) {
    if (isSecondary) {
        bindings[action].keySecondary = newKey; // Modifie la colonne de droite
    } else {
        bindings[action].keyPrimary = newKey;   // Modifie la colonne de gauche
    }
}

// Récupère le code de la touche (pour l'affichage dans le menu)
SDL_Keycode GetKeyBinding(GameAction action, bool secondary) {
    if (secondary) return bindings[action].keySecondary; // Retourne la touche secondaire
    return bindings[action].keyPrimary;                 // Retourne la touche principale
}

// --- SAUVEGARDE ---

// Ecrit la config touches dans le fichier
void SaveInputProfile(FILE* f) {
    // Ecrit tout le tableau de touches d'un coup
    fwrite(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
}

// Lit la config touches depuis le fichier
void LoadInputProfile(FILE* f) {
    // Remplit le tableau avec ce qui est dans le fichier
    fread(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
}