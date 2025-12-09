#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <SDL.h>
#include <stdio.h> // Nécessaire pour FILE*

typedef enum {
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_A,
    ACTION_E,
    ACTION_C,
    ACTION_MUTE,
    ACTION_VOL_UP,
    ACTION_VOL_DOWN,
    ACTION_COUNT
} GameAction;

typedef struct {
    SDL_Keycode keyPrimary;
    SDL_Keycode keySecondary;
} KeyBinding;

void InitInputProfile();
bool IsActionPressed(GameAction action, int keyPressed);

void UpdateKeyBinding(GameAction action, SDL_Keycode newKey, bool isSecondary);
SDL_Keycode GetKeyBinding(GameAction action, bool secondary);

// NOUVEAU : Fonctions pour sauvegarder/charger les touches
void SaveInputProfile(FILE* f);
void LoadInputProfile(FILE* f);

#endif