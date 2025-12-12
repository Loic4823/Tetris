#include "input.h"
#include <SDL.h>
#include <stdbool.h> // Pour bool, true, false
#include <stdio.h>   // Pour FILE, fopen, fwrite, etc.

// Tableau global des bindings
KeyBinding bindings[ACTION_COUNT]; 

void InitInputProfile() {
    // Configuration par défaut
    bindings[ACTION_LEFT].keyPrimary = SDLK_LEFT;
    bindings[ACTION_LEFT].keySecondary = SDLK_q;
    
    bindings[ACTION_RIGHT].keyPrimary = SDLK_RIGHT;
    bindings[ACTION_RIGHT].keySecondary = SDLK_d;
    
    bindings[ACTION_DOWN].keyPrimary = SDLK_DOWN;
    bindings[ACTION_DOWN].keySecondary = SDLK_s;
    
    bindings[ACTION_UP].keyPrimary = SDLK_UP;
    bindings[ACTION_UP].keySecondary = SDLK_z;
    
    bindings[ACTION_A].keyPrimary = SDLK_a;     // Rotation gauche
    bindings[ACTION_A].keySecondary = 0;        // Pas de secondaire
    
    bindings[ACTION_E].keyPrimary = SDLK_e;     // Rotation droite
    bindings[ACTION_E].keySecondary = 0;
    
    bindings[ACTION_C].keyPrimary = SDLK_c;     // Hold
    bindings[ACTION_C].keySecondary = 0;
}

// Vérifie si une action correspond à la touche pressée
// J'ai changé le type de retour en int (1 ou 0) pour simplifier, 
// ou on peut garder bool si stdbool.h est bien inclus.
int IsActionPressed(GameAction action, SDL_Keycode keyPressed) {
    if (action < 0 || action >= ACTION_COUNT) return 0;
    if (keyPressed == bindings[action].keyPrimary) return 1;
    if (keyPressed == bindings[action].keySecondary) return 1;
    return 0;
}

void UpdateKeyBinding(GameAction action, SDL_Keycode newKey, int isSecondary) {
    if (action < 0 || action >= ACTION_COUNT) return;
    
    if (isSecondary) {
        bindings[action].keySecondary = newKey;
    } else {
        bindings[action].keyPrimary = newKey;
    }
}

SDL_Keycode GetKeyBinding(GameAction action, int isSecondary) {
    if (action < 0 || action >= ACTION_COUNT) return SDLK_UNKNOWN;
    
    if (isSecondary) {
        return bindings[action].keySecondary;
    } else {
        return bindings[action].keyPrimary;
    }
}

void SaveKeyConfig() {
    FILE* f = fopen("keyconfig.dat", "wb");
    if (f) {
        fwrite(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
        fclose(f);
    }
}

void LoadKeyConfig() {
    FILE* f = fopen("keyconfig.dat", "rb");
    if (f) {
        fread(bindings, sizeof(KeyBinding), ACTION_COUNT, f);
        fclose(f);
    } else {
        InitInputProfile(); // Si pas de fichier, charger defauts
    }
}

// Cette fonction n'est plus utilisée comme telle dans la logique actuelle,
// mais on la laisse vide pour compatibilité si nécessaire, ou on la supprime.
void HandleKeyConfigInput(GameContext* game, SDL_Keycode key) {
    if (key == SDLK_ESCAPE) {
        game->state = STATE_SETTINGS;
        game->isRebinding = 0;
        return;
    }

    if (game->isRebinding) {
        // Appliquer la nouvelle touche
        GameAction actionToRebind = (GameAction)game->keyConfigSelection; // Cast sûr car selection correspond aux indices
        UpdateKeyBinding(actionToRebind, key, game->keyConfigColumn);
        game->isRebinding = 0;
    } else {
        // Navigation dans le menu
        if (key == SDLK_UP) {
            game->keyConfigSelection--;
            if (game->keyConfigSelection < 0) game->keyConfigSelection = ACTION_COUNT - 1;
        } else if (key == SDLK_DOWN) {
            game->keyConfigSelection++;
            if (game->keyConfigSelection >= ACTION_COUNT) game->keyConfigSelection = 0;
        } else if (key == SDLK_LEFT) {
            game->keyConfigColumn = 0;
        } else if (key == SDLK_RIGHT) {
            game->keyConfigColumn = 1;
        } else if (key == SDLK_RETURN) {
            game->isRebinding = 1;
        }
    }
}