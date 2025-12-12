#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include "defs.h" 

void InitInputProfile();
void HandleKeyConfigInput(GameContext* game, SDL_Keycode key);
SDL_Keycode GetKeyBinding(GameAction action, int secondary);
int IsActionPressed(GameAction action, SDL_Keycode key); // Retourne int (0 ou 1)
void UpdateKeyBinding(GameAction action, SDL_Keycode newKey, int isSecondary);
void LoadKeyConfig();
void SaveKeyConfig();

#endif