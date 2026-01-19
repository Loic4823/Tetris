#ifndef LOGIC_H
#define LOGIC_H

#include "defs.h"

// Prépare une nouvelle partie (reset score, grille, sac de pièces).
void initGame(GameContext* game);
void resetGameLogic(GameContext* game);

// [IMPORTANT] La boucle de mise à jour logique.
// 'deltaTime' est le temps écoulé depuis la dernière frame.
// C'est crucial pour gérer la descente des pièces (gravité) et les timers (DAS, Lock Delay)
// de manière indépendante de la puissance du processeur (FPS).
void updateGame(GameContext* game, Uint32 deltaTime);

// Gère l'appui initial sur une touche (Down).
void handleInput(GameContext* game, SDL_Keycode key);

// [NOUVEAU] Gère le relâchement d'une touche (Up).
// Indispensable pour arrêter le DAS (défilement rapide) quand on lâche la flèche.
void handleInputUp(GameContext* game, SDL_Keycode key); 

// Pioche une pièce dans le "sac" (Bag System) pour éviter les répétitions trop fréquentes.
int getRandomPieceType(GameContext* game);

// Sauvegarde/Chargement des données persistantes (High Score, Config touches).
void saveGameData(GameContext* game, int saveFullState);
void loadGameData(GameContext* game);

#endif