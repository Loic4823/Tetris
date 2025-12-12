#include "defs.h"
#include "logic.h"
#include "draw.h"
#include "input.h"
#include "audio.h" 
#include <stdio.h>

int main(int argc, char* argv[]) {
    AppContext app = {0};
    GameContext game;
    
    // L'audio est maintenant initialisé ici, à l'intérieur de initSDL
    if (!initSDL(&app)) {
        printf("Erreur init SDL\n");
        return 1;
    }
    
    LoadKeyConfig(); 
    initGame(&game);
    
    // J'ai SUPPRIMÉ la ligne "initAudioSystem();" qui causait l'erreur
    
    Uint32 lastTime = SDL_GetTicks();
    int running = 1;
    
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } 
            else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                handleInput(&game, key);
            }
            else if (event.type == SDL_KEYUP) {
                SDL_Keycode key = event.key.keysym.sym;
                handleKeyUp(&game, key);
            }
        }
        
        // Appliquer la résolution si changée dans le menu
        static int currentRes = 0;
        if (game.menuResolution != currentRes) {
            applyWindowResolution(&app, game.menuResolution);
            currentRes = game.menuResolution;
        }

        // Changer la musique si changée dans le menu
        static int currentTrack = -1;
        if (game.menuMusicTrack != currentTrack) {
            playMusicTrack(&app, game.menuMusicTrack);
            currentTrack = game.menuMusicTrack;
        }
        
        // Appliquer le volume
        applyAudioVolume(&game); 

        updateGame(&game, deltaTime);
        
        if (game.state == STATE_MENU) renderMenu(&app, &game);
        else if (game.state == STATE_SETTINGS || game.state == STATE_KEY_CONFIG) renderMenu(&app, &game);
        else renderGame(&app, &game);
        
        SDL_Delay(16); 
    }
    
    SaveKeyConfig(); 
    cleanupSDL(&app);
    return 0;
}