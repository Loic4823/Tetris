#ifndef DRAW_H
#define DRAW_H

#include "defs.h" // Besoin de la structure AppContext.

int initSDL(AppContext* app);      // Démarre la fenêtre.
void cleanupSDL(AppContext* app);  // Ferme tout proprement.
void loadBlockTextures(AppContext* app, int style); // Charge les images des blocs (MC, Default...).

void playMusicTrack(AppContext* app, int track); // Lance la musique.
void playClearSound(AppContext* app); // Joue le son "Clear".
void applyAudioVolume(GameContext* game); // Met à jour le volume.
void applyWindowResolution(AppContext* app, int mode); // Change la résolution.

void renderGame(AppContext* app, GameContext* game); // Dessine le jeu.
void renderMenu(AppContext* app, GameContext* game); // Dessine le menu principal.
void renderKeyConfigMenu(AppContext* app, GameContext* game); // Dessine config touches.
void renderSettingsMenu(AppContext* app, GameContext* game); // Dessine les paramètres.

#endif