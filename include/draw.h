#ifndef DRAW_H
#define DRAW_H

#include "defs.h" 

// Initialise SDL, la fenêtre, le renderer et les polices.
int initSDL(AppContext* app);      
// Nettoie la mémoire (textures, sons) à la fermeture.
void cleanupSDL(AppContext* app);  

// Charge les images des blocs selon le style choisi (ex: Retro vs Moderne).
void loadBlockTextures(AppContext* app, int style); 

void playMusicTrack(AppContext* app, int track); 
void playClearSound(AppContext* app); 
void applyWindowResolution(AppContext* app, int mode); 

// [IMPORTANT] Fonction principale de dessin.
// Elle prend 'GameContext' (données logiques) et dessine sur 'AppContext' (fenêtre/renderer).
void renderGame(AppContext* app, GameContext* game); 

void renderMenu(AppContext* app, GameContext* game); 
void renderKeyConfigMenu(AppContext* app, GameContext* game); 
void renderSettingsMenu(AppContext* app, GameContext* game); 

#endif