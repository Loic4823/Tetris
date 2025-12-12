#ifndef LOGIC_H
#define LOGIC_H

#include "defs.h"

void initGame(GameContext* game);
void resetGameLogic(GameContext* game);
void updateGame(GameContext* game, Uint32 deltaTime);
void handleInput(GameContext* game, SDL_Keycode key);
void handleInputUp(GameContext* game, SDL_Keycode key); // <--- AJOUT POUR LE DAS
int getRandomPieceType(GameContext* game);

void saveGameData(GameContext* game, int saveFullState);
void loadGameData(GameContext* game);

#endif