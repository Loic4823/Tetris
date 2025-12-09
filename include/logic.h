#ifndef LOGIC_H
#define LOGIC_H

#include "defs.h"

void initGame(GameContext* game);
void resetGameLogic(GameContext* game);
void updateGame(GameContext* game, Uint32 deltaTime);
void handleInput(GameContext* game, SDL_Keycode key);
int getRandomPieceType();

#endif