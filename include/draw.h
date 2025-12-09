#ifndef DRAW_H
#define DRAW_H

#include "defs.h"

int initSDL(AppContext* app);
void cleanupSDL(AppContext* app);
void loadBlockTextures(AppContext* app, int style);
void playMusicTrack(AppContext* app, int track);
void applyAudioVolume(GameContext* game);
void applyWindowResolution(AppContext* app, int mode);
void renderGame(AppContext* app, GameContext* game);
void renderMenu(AppContext* app, GameContext* game);
void renderKeyConfigMenu(AppContext* app, GameContext* game);
void renderSettingsMenu(AppContext* app, GameContext* game);

// NOUVEAU : Fonction pour jouer le son
void playClearSound(AppContext* app);

#endif