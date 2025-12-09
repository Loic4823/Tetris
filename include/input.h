#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h> // Pour bool.
#include <SDL.h>     // Pour les codes touches (SDLK_...).
#include <stdio.h>   // Pour FILE* (sauvegarde).

// Liste des actions possibles dans le jeu.
typedef enum {
    ACTION_UP,       // Haut (souvent Hard Drop).
    ACTION_DOWN,     // Bas (Soft Drop).
    ACTION_LEFT,     // Gauche.
    ACTION_RIGHT,    // Droite.
    ACTION_A,        // Rotation 1.
    ACTION_E,        // Rotation 2.
    ACTION_C,        // Hold (Réserve).
    ACTION_MUTE,     // Couper son.
    ACTION_VOL_UP,   // Monter son.
    ACTION_VOL_DOWN, // Baisser son.
    ACTION_COUNT     // Astuce : c'est le nombre total d'actions.
} GameAction;

// Une action peut avoir 2 touches associées (ex: Z et Flèche Haut).
typedef struct {
    SDL_Keycode keyPrimary;   // Touche principale.
    SDL_Keycode keySecondary; // Touche secondaire.
} KeyBinding;

void InitInputProfile(); // Règle les touches par défaut.
bool IsActionPressed(GameAction action, int keyPressed); // Vérifie si une touche correspond à une action.

// Change une touche pour une action donnée.
void UpdateKeyBinding(GameAction action, SDL_Keycode newKey, bool isSecondary);
// Récupère le code d'une touche pour l'afficher.
SDL_Keycode GetKeyBinding(GameAction action, bool secondary);

// Sauvegarde et chargement des touches dans un fichier.
void SaveInputProfile(FILE* f);
void LoadInputProfile(FILE* f);

#endif