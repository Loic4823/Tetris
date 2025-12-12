#include <SDL.h>
#include <stdio.h>
#include "defs.h"
#include "draw.h"
#include "logic.h"
#include "input.h"
#include "audio.h"

int main(int argc, char *argv[]) {
    AppContext app = {0};
    GameContext game = {0}; 

    InitInputProfile();
    
    if (!initSDL(&app)) {
        fprintf(stderr, "Erreur Init SDL\n");
        return 1;
    }
    
    InitAudio(); // Le son démarre à 0 (voir audio.c)
    initGame(&game);
    playMusicTrack(&app, game.menuMusicTrack);

    Uint32 lastTime = SDL_GetTicks();
    int running = 1;

    while (running && game.state != STATE_QUIT) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                handleInput(&game, key);
            }
            // --- GESTION DU RELACHEMENT (Pour arrêter le DAS) ---
            else if (event.type == SDL_KEYUP) {
                SDL_Keycode key = event.key.keysym.sym;
                handleInputUp(&game, key);
            }
            // ----------------------------------------------------
        }

        updateGame(&game, deltaTime);

        if (game.reloadAssetsPending) {
            loadBlockTextures(&app, game.menuTextureStyle);
            game.reloadAssetsPending = 0;
        }

        if (game.changeMusicPending) {
            playMusicTrack(&app, game.menuMusicTrack);
            game.changeMusicPending = 0;
        }

        if (game.changeResolutionPending) {
            applyWindowResolution(&app, game.menuResolution);
            game.changeResolutionPending = 0;
        }
        
        if (game.playSoundClearPending) {
            playClearSound(&app);
            game.playSoundClearPending = 0;
        }

        // Mise à jour des variables d'affichage audio
        game.masterVolume = (int)(audioSystem.volume * 128);
        game.isMuted = audioSystem.isMuted;

        if (game.state == STATE_MENU || game.state == STATE_KEY_CONFIG || game.state == STATE_SETTINGS) {
            renderMenu(&app, &game);
        } else {
            renderGame(&app, &game);
        }

        SDL_Delay(16);
    }

    // Sauvegarde complète en quittant
    if (game.gameInProgress) {
        saveGameData(&game, 1); // Sauvegarde tout (grille incluse)
    } else {
        saveGameData(&game, 0); // Sauvegarde juste les settings
    }

    cleanupSDL(&app);
    return 0;
}