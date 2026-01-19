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
    
    // [IMPORTANT] Initialisation du système graphique
    if (!initSDL(&app)) {
        fprintf(stderr, "Erreur Init SDL\n");
        return 1;
    }
    
    InitAudio();
    initGame(&game);
    playMusicTrack(&app, game.menuMusicTrack);

    // [LOGIQUE TEMPORELLE] 'lastTime' sert à mesurer combien de temps dure une frame.
    Uint32 lastTime = SDL_GetTicks();
    int running = 1;

    // --- BOUCLE PRINCIPALE (GAME LOOP) ---
    while (running && game.state != STATE_QUIT) {
        
        // [DELTA TIME] Calcul du temps écoulé depuis la dernière boucle.
        // C'est CRUCIAL : si l'ordinateur est lent, deltaTime est grand. S'il est rapide, il est petit.
        // On passe cette valeur à 'updateGame' pour que la physique soit constante peu importe les FPS.
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 1. GESTION DES ÉVÉNEMENTS (Clavier/Souris)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                handleInput(&game, key); // Appui touche
            }
            else if (event.type == SDL_KEYUP) {
                SDL_Keycode key = event.key.keysym.sym;
                handleInputUp(&game, key); // Relâchement touche (Important pour arrêter le DAS)
            }
        }

        // 2. MISE À JOUR LOGIQUE (Physique, Timers)
        updateGame(&game, deltaTime);

        // --- GESTION DES DRAPEAUX (FLAGS) ---
        // Le moteur logique demande des changements, le moteur graphique les exécute ici.
        // Cela évite de mélanger la logique pur et les appels SDL lourds.

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

        // Synchro audio entre logique et système
        game.masterVolume = (int)(audioSystem.volume * 128);
        game.isMuted = audioSystem.isMuted;

        // 3. RENDU GRAPHIQUE
        if (game.state == STATE_MENU || game.state == STATE_KEY_CONFIG || game.state == STATE_SETTINGS) {
            renderMenu(&app, &game);
        } else {
            renderGame(&app, &game);
        }

        // Petite pause pour ne pas utiliser 100% du CPU inutilement (limite ~60 FPS)
        SDL_Delay(16);
    }

    // [SAUVEGARDE] À la fermeture
    if (game.gameInProgress) {
        saveGameData(&game, 1); // Sauvegarde complète si jeu en cours
    } else {
        saveGameData(&game, 0); // Sauvegarde juste le score/options
    }

    cleanupSDL(&app);
    return 0;
}