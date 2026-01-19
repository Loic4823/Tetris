#include "draw.h"
#include "audio.h"
#include "input.h"
#include <SDL_image.h>
#include <stdio.h>
#include <math.h>

// On a besoin des formes géométriques définies ailleurs pour dessiner les previews et le fantôme.
extern const int TETROMINO_SHAPES[7][4][4][2];

// --- GESTION DES TEXTURES ---

// Helper : Charge une image depuis le disque vers la VRAM (Mémoire Vidéo).
static SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path) {
    // 1. Charge l'image dans la RAM (CPU) -> Surface
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) return NULL;
    
    // 2. Transfère l'image vers la VRAM (GPU) -> Texture
    // C'est beaucoup plus rapide à afficher par la suite.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // 3. Libère la RAM (la surface ne sert plus une fois la texture créée)
    SDL_FreeSurface(surface);
    return texture;
}

// Charge ou Recharge les skins des blocs (Default, Alt, Minecraft).
void loadBlockTextures(AppContext* app, int style) {
    char path[128];
    const char* styleFolder;
    if (style == 0) styleFolder = "default";
    else if (style == 1) styleFolder = "alt";
    else styleFolder = "mc";

    const char* colors[] = {"cyan", "jaune", "violet", "vert", "rouge", "bleu", "orange"};

    for (int i = 0; i < 7; i++) {
        // [CRITIQUE] : Si une texture existait déjà (changement de style en cours de jeu),
        // il faut impérativement la détruire avant d'en charger une nouvelle pour éviter les fuites de mémoire (VRAM leak).
        if (app->blockTextures[i]) SDL_DestroyTexture(app->blockTextures[i]);
        
        sprintf(path, "assets/images/%s/%s.png", styleFolder, colors[i]);
        app->blockTextures[i] = loadTexture(app->renderer, path);
        
        if (!app->blockTextures[i]) printf("Erreur chargement texture: %s\n", path);
    }
}

// --- INITIALISATION DU SYSTÈME ---

int initSDL(AppContext* app) {
    // 1. Initialisation des sous-systèmes SDL (Vidéo et Audio)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 0;
    }
    // 2. Initialisation des polices (TTF)
    if (TTF_Init() == -1) return 0;
    // 3. Initialisation du chargeur d'images (PNG)
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 0;
    
    // 4. Initialisation du mixeur audio (MP3)
    int flags = MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        printf("Attention Mix_Init: %s\n", Mix_GetError());
    }

    // Ouverture du périphérique audio : 44.1kHz, Stéréo, Buffer 2048 (latence vs qualité)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mix_OpenAudio: %s\n", Mix_GetError());
        return 0;
    }

    // Création de la fenêtre
    app->window = SDL_CreateWindow("Tetris C SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LOGICAL_WIDTH, LOGICAL_HEIGHT, SDL_WINDOW_SHOWN);
    if (!app->window) return 0;

    // Création du Renderer (le "Peintre" accéléré par la carte graphique)
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);
    if (!app->renderer) return 0;

    // [IMPORTANT] Indépendance de la résolution.
    // On dit à SDL : "Fais comme si l'écran faisait 800x700".
    // Si la fenêtre est agrandie ou en plein écran, SDL étirera tout automatiquement.
    SDL_RenderSetLogicalSize(app->renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);

    // Chargement des assets initiaux
    loadBlockTextures(app, 0);
    app->ghostTexture = loadTexture(app->renderer, "assets/images/ghost.png");
    app->menuBackground = loadTexture(app->renderer, "assets/images/menu.png");
    app->settingsBackground = loadTexture(app->renderer, "assets/images/parametre.png");

    app->fontLarge = TTF_OpenFont("assets/fonts/SourceCodePro-Bold.ttf", 28);
    app->fontSmall = TTF_OpenFont("assets/fonts/SourceCodePro-Bold.ttf", 14);

    // Pré-chargement des musiques (Streaming)
    char zikPath[64];
    for (int i = 0; i < 11; i++) {
        sprintf(zikPath, "assets/music/tetris%d.mp3", i + 1);
        app->musics[i] = Mix_LoadMUS(zikPath);
        if (!app->musics[i]) {
            printf("ERREUR: Impossible de charger %s -> %s\n", zikPath, Mix_GetError());
        }
    }

    // Chargement du bruitage (Chunk mis en cache RAM)
    app->soundClear = Mix_LoadWAV("assets/music/clear.mp3");
    if (!app->soundClear) {
        printf("ERREUR: Impossible de charger clear.mp3 -> %s\n", Mix_GetError());
    }

    return 1;
}

// --- AUDIO ---

void playMusicTrack(AppContext* app, int track) {
    Mix_HaltMusic(); // Arrête proprement la piste précédente
    if (track >= 0 && track < 11 && app->musics[track]) {
        Mix_PlayMusic(app->musics[track], -1); // -1 = Boucle infinie
    }
}

void playClearSound(AppContext* app) {
    if (app->soundClear) {
        Mix_PlayChannel(-1, app->soundClear, 0); // Joue sur le premier canal dispo
    }
}

// --- FENÊTRE ---

void applyWindowResolution(AppContext* app, int mode) {
    if (mode == 0) {
        SDL_SetWindowFullscreen(app->window, 0);
        SDL_SetWindowSize(app->window, 800, 700);
    } else if (mode == 1) {
        SDL_SetWindowFullscreen(app->window, 0);
        SDL_SetWindowSize(app->window, 1200, 1050);
    } else if (mode == 2) {
        SDL_SetWindowFullscreen(app->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    // Ré-applique la logique d'échelle après redimensionnement pour éviter les déformations
    SDL_RenderSetLogicalSize(app->renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
    SDL_SetWindowPosition(app->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

// --- NETTOYAGE (CLEANUP) ---

void cleanupSDL(AppContext* app) {
    // Il est vital de libérer toute la mémoire allouée manuellement.
    for (int i = 0; i < 7; i++) if (app->blockTextures[i]) SDL_DestroyTexture(app->blockTextures[i]);
    if (app->ghostTexture) SDL_DestroyTexture(app->ghostTexture);
    if (app->menuBackground) SDL_DestroyTexture(app->menuBackground);
    if (app->settingsBackground) SDL_DestroyTexture(app->settingsBackground);
    if (app->fontLarge) TTF_CloseFont(app->fontLarge);
    if (app->fontSmall) TTF_CloseFont(app->fontSmall);

    for (int i = 0; i < 11; i++) if (app->musics[i]) Mix_FreeMusic(app->musics[i]);
    if (app->soundClear) Mix_FreeChunk(app->soundClear);

    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

// --- FONCTIONS DE DESSIN (RENDERING) ---

// Affiche du texte. Créer une texture à chaque frame est coûteux mais simple.
// Pour optimiser, on pourrait mettre en cache les textes statiques.
static void renderText(AppContext* app, const char* text, int x, int y, SDL_Color color, TTF_Font* font, int center) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(app->renderer, surface);
    
    SDL_Rect dst = { x, y, surface->w, surface->h };
    if (center) dst.x -= surface->w / 2; // Centrage horizontal
    
    SDL_RenderCopy(app->renderer, texture, NULL, &dst);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Dessine un bloc unique (soit texture, soit carré plein si texture manquante).
static void renderBlock(AppContext* app, int x, int y, int type, int isGhost) {
    // Conversion Coordonnées Grille -> Coordonnées Pixels
    SDL_Rect rect = { BOARD_X_OFFSET + x * BLOCK_SIZE, BOARD_Y_OFFSET + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
    
    if (isGhost) {
        // Le fantôme est semi-transparent ou filaire
        if (app->ghostTexture) SDL_RenderCopy(app->renderer, app->ghostTexture, NULL, &rect);
        else { SDL_SetRenderDrawColor(app->renderer, 200, 200, 200, 100); SDL_RenderDrawRect(app->renderer, &rect); }
    } else {
        if (type >= 0 && type < 7 && app->blockTextures[type]) SDL_RenderCopy(app->renderer, app->blockTextures[type], NULL, &rect);
        else { SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255); SDL_RenderFillRect(app->renderer, &rect); }
    }
}

// Affiche les petites grilles "Next" et "Hold".
static void renderPiecePreview(AppContext* app, int type, int x, int y) {
    if (type == -1) return;
    for (int i = 0; i < 4; i++) {
        int bx = TETROMINO_SHAPES[type][0][i][1];
        int by = TETROMINO_SHAPES[type][0][i][0];
        SDL_Rect rect = { x + bx * BLOCK_SIZE, y + by * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
        if (app->blockTextures[type]) SDL_RenderCopy(app->renderer, app->blockTextures[type], NULL, &rect);
    }
}

// [GHOST PIECE] Simulation visuelle
static void renderGhost(AppContext* app, GameContext* game) {
    int ghostY = game->currentPiece.y;
    int collision = 0;
    
    // 1. On fait descendre une copie virtuelle de la pièce jusqu'à toucher quelque chose
    while (!collision) {
        ghostY++;
        for (int i = 0; i < 4; i++) {
            int bx = game->currentPiece.x + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][1];
            int by = ghostY + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][0];
            // Collision murs ou blocs
            if (bx < 0 || bx >= BOARD_WIDTH || by >= BOARD_HEIGHT || (by >= 0 && game->board[by][bx] != -1)) collision = 1;
        }
    }
    ghostY--; // On remonte de 1 car on est rentré "dans" l'obstacle
    
    // 2. On dessine le résultat
    for (int i = 0; i < 4; i++) {
        int bx = game->currentPiece.x + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][1];
        int by = ghostY + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][0];
        if (by >= 0) renderBlock(app, bx, by, -1, 1); // -1 = Type Ghost
    }
}

// --- BOUCLE DE RENDU PRINCIPALE ---
// Cette fonction est appelée ~60 fois par seconde.
void renderGame(AppContext* app, GameContext* game) {
    // 1. CLEAR : On efface l'écran précédent
    SDL_SetRenderDrawColor(app->renderer, 16, 16, 26, 255);
    SDL_RenderClear(app->renderer);

    // 2. BACKGROUND : Dessin du fond du plateau
    SDL_SetRenderDrawColor(app->renderer, 15, 15, 30, 255);
    SDL_Rect boardRect = { BOARD_X_OFFSET, BOARD_Y_OFFSET, BOARD_WIDTH * BLOCK_SIZE, BOARD_HEIGHT * BLOCK_SIZE };
    SDL_RenderFillRect(app->renderer, &boardRect);

    // 3. GRID : Dessin des lignes de grille
    SDL_SetRenderDrawColor(app->renderer, 64, 64, 96, 128);
    for (int x = 0; x <= BOARD_WIDTH; x++) SDL_RenderDrawLine(app->renderer, BOARD_X_OFFSET + x * BLOCK_SIZE, BOARD_Y_OFFSET, BOARD_X_OFFSET + x * BLOCK_SIZE, BOARD_Y_OFFSET + BOARD_HEIGHT * BLOCK_SIZE);
    for (int y = 0; y <= BOARD_HEIGHT; y++) SDL_RenderDrawLine(app->renderer, BOARD_X_OFFSET, BOARD_Y_OFFSET + y * BLOCK_SIZE, BOARD_X_OFFSET + BOARD_WIDTH * BLOCK_SIZE, BOARD_Y_OFFSET + y * BLOCK_SIZE);

    // 4. BOARD : Dessin des pièces déjà posées
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x] != -1) renderBlock(app, x, y, game->board[y][x], 0);
        }
    }

    // 5. ACTIVE PIECE : Dessin de la pièce qui tombe + Fantôme
    if (game->state == STATE_PLAYING) {
        renderGhost(app, game);
        for (int i = 0; i < 4; i++) {
            int bx = game->currentPiece.x + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][1];
            int by = game->currentPiece.y + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][0];
            if (by >= 0) renderBlock(app, bx, by, game->currentPiece.type, 0);
        }
    }

    // 6. ANIMATION : Flash blanc sur les lignes complétées
    if (game->state == STATE_ANIMATING) {
        // Math : Sinusoïde pour effet de "pulsation" de la transparence (Alpha)
        int alpha = (int)(sin(game->animTimer * 0.02) * 100 + 100);
        SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, alpha);
        for (int i = 0; i < game->linesToClearCount; i++) {
            int y = game->linesToClear[i];
            SDL_Rect lineRect = { BOARD_X_OFFSET, BOARD_Y_OFFSET + y * BLOCK_SIZE, BOARD_WIDTH * BLOCK_SIZE, BLOCK_SIZE };
            SDL_RenderFillRect(app->renderer, &lineRect);
        }
    }

    // 7. UI : Scores, Textes
    int uiX = 490;
    SDL_Color white = {255, 255, 255, 255};
    char buffer[64];

    renderText(app, "Suivant:", uiX, 80, white, app->fontLarge, 0);
    renderPiecePreview(app, game->nextPieceType, uiX, 120);

    renderText(app, "Reserve (C):", uiX, 250, white, app->fontLarge, 0);
    renderPiecePreview(app, game->heldPieceType, uiX, 290);

    sprintf(buffer, "Score: %d", game->score);
    renderText(app, buffer, uiX, 450, white, app->fontLarge, 0);

    sprintf(buffer, "Lignes: %d", game->linesCleared);
    renderText(app, buffer, uiX, 490, white, app->fontLarge, 0);

    sprintf(buffer, "Niveau: %d", game->level);
    renderText(app, buffer, uiX, 530, white, app->fontLarge, 0);

    // Messages Flottants (T-Spin, Tetris)
    if (game->messageTimer > 0) {
        game->messageTimer--;
        
        // Clignotement Or/Blanc
        SDL_Color msgColor = {255, 255, 255, 255};
        if (game->messageTimer % 10 < 5) {
             msgColor.r = 255; msgColor.g = 215; msgColor.b = 0; 
        }

        renderText(app, game->messageText, BOARD_X_OFFSET + (BOARD_WIDTH * BLOCK_SIZE) / 2, 250, msgColor, app->fontLarge, 1);
    }

    renderText(app, "Voir Menu pour config touches | Espace: Pause", LOGICAL_WIDTH / 2, LOGICAL_HEIGHT - 30, white, app->fontSmall, 1);

    // 8. OVERLAYS : Pause ou Game Over par dessus le jeu
    if (game->state == STATE_GAMEOVER) {
        SDL_SetRenderDrawColor(app->renderer, 50, 50, 50, 200);
        SDL_Rect overlay = { BOARD_X_OFFSET, 300, BOARD_WIDTH * BLOCK_SIZE, 100 };
        SDL_RenderFillRect(app->renderer, &overlay);
        SDL_Color yellow = {255, 255, 0, 255};
        renderText(app, "GAME OVER", BOARD_X_OFFSET + (BOARD_WIDTH * BLOCK_SIZE) / 2, 320, yellow, app->fontLarge, 1);
    }
    if (game->isPaused && game->state == STATE_PLAYING) {
        SDL_Color pauseColor = {200, 200, 200, 255};
        renderText(app, "PAUSE", BOARD_X_OFFSET + (BOARD_WIDTH * BLOCK_SIZE) / 2, 320, pauseColor, app->fontLarge, 1);
    }

    // 9. PRESENT : On affiche le buffer qu'on vient de dessiner (Double Buffering)
    SDL_RenderPresent(app->renderer);
}

// --- MENUS ---

void renderKeyConfigMenu(AppContext* app, GameContext* game) {
    // Nettoyage fond
    SDL_SetRenderDrawColor(app->renderer, 20, 20, 30, 255);
    SDL_RenderClear(app->renderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {255, 255, 0, 255};
    SDL_Color waitingColor = {255, 0, 0, 255}; // Rouge si en attente d'input

    int midX = LOGICAL_WIDTH / 2;
    int startY = 100;
    int spacing = 50;

    renderText(app, "CONFIGURATION TOUCHES", midX, 40, white, app->fontLarge, 1);
    renderText(app, "Entree pour modifier, Fleches pour naviguer", midX, 80, white, app->fontSmall, 1);

    const char* actions[] = {"Gauche", "Droite", "Bas (Douce)", "Haut (Dur)", "Rot. Gauche", "Rot. Droite", "Reserve"};
    GameAction actionTypes[] = {ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_UP, ACTION_A, ACTION_E, ACTION_C};

    // Boucle d'affichage de la grille de touches
    for (int i = 0; i < 7; i++) {
        SDL_Keycode key1 = GetKeyBinding(actionTypes[i], false);
        SDL_Keycode key2 = GetKeyBinding(actionTypes[i], true);
        char buffer1[64];
        char buffer2[64];

        const char* rawName1 = SDL_GetKeyName(key1);
        // Ajout de crochets si sélectionné
        if (game->keyConfigSelection == i && game->keyConfigColumn == 0) sprintf(buffer1, "[ %s ]", rawName1);
        else sprintf(buffer1, "%s", rawName1);

        if (key2 == 0) {
            if (game->keyConfigSelection == i && game->keyConfigColumn == 1) sprintf(buffer2, "[ ... ]");
            else sprintf(buffer2, "...");
        } else {
            const char* rawName2 = SDL_GetKeyName(key2);
            if (game->keyConfigSelection == i && game->keyConfigColumn == 1) sprintf(buffer2, "[ %s ]", rawName2);
            else sprintf(buffer2, "%s", rawName2);
        }

        renderText(app, actions[i], 200, startY + i * spacing, white, app->fontLarge, 0);

        // Gestion couleur (Blanc = Normal, Jaune = Sélect, Rouge = En cours de modif)
        SDL_Color c1 = (game->keyConfigSelection == i && game->keyConfigColumn == 0) ? highlight : white;
        if (game->keyConfigSelection == i && game->keyConfigColumn == 0 && game->isRebinding) c1 = waitingColor;
        renderText(app, buffer1, 500, startY + i * spacing, c1, app->fontLarge, 1);

        SDL_Color c2 = (game->keyConfigSelection == i && game->keyConfigColumn == 1) ? highlight : white;
        if (game->keyConfigSelection == i && game->keyConfigColumn == 1 && game->isRebinding) c2 = waitingColor;
        renderText(app, buffer2, 700, startY + i * spacing, c2, app->fontLarge, 1);
    }
    SDL_RenderPresent(app->renderer);
}

void renderSettingsMenu(AppContext* app, GameContext* game) {
    SDL_SetRenderDrawColor(app->renderer, 16, 16, 26, 255);
    SDL_RenderClear(app->renderer);

    if (app->settingsBackground) SDL_RenderCopy(app->renderer, app->settingsBackground, NULL, NULL);

    // Décoration : affiche des pièces autour du menu
    renderPiecePreview(app, 0, 50, 150);
    renderPiecePreview(app, 1, 650, 150);
    renderPiecePreview(app, 3, 620, 300);
    renderPiecePreview(app, 2, 80, 300);
    renderPiecePreview(app, 5, 50, 600);
    renderPiecePreview(app, 4, 650, 450);
    renderPiecePreview(app, 6, 50, 450);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {255, 255, 0, 255};
    int midX = LOGICAL_WIDTH / 2;
    int startY = 100;
    int spacing = 50;
    char buffer[64];

    renderText(app, "PARAMETRES", midX, 40, white, app->fontLarge, 1);

    // Affichage conditionnel des options (Texture, Vitesse, Niveau, etc.)
    char* texName = "Default";
    if (game->menuTextureStyle == 1) texName = "Alt";
    else if (game->menuTextureStyle == 2) texName = "MC";
    sprintf(buffer, "Texture: < %s >", texName);
    renderText(app, buffer, midX, startY, (game->menuSelectedOption == 0) ? highlight : white, app->fontLarge, 1);

    sprintf(buffer, "Vitesse Auto: [%c]", game->menuAutoSpeed ? 'X' : ' ');
    renderText(app, buffer, midX, startY + spacing, (game->menuSelectedOption == 1) ? highlight : white, app->fontLarge, 1);

    sprintf(buffer, "Niveau Depart: < %d >", game->menuStartLevel);
    renderText(app, buffer, midX, startY + spacing * 2, (game->menuSelectedOption == 2) ? highlight : white, app->fontLarge, 1);

    if (audioSystem.isMuted || audioSystem.volume <= 0.01f) sprintf(buffer, "Volume: < MUET >");
    else sprintf(buffer, "Volume: < %d%% >", (int)(audioSystem.volume * 100));
    renderText(app, buffer, midX, startY + spacing * 3, (game->menuSelectedOption == 3) ? highlight : white, app->fontLarge, 1);

    const char* musicNames[] = { 
        "Electro", "Orchestre", "Dramatique", "Techno", 
        "Orgue Vent", "Metal", "Bass Techno", "Flute", 
        "Piano", "Japonais", "Minecraft", "Aucune" 
    };

    int trackIndex = game->menuMusicTrack;
    if (trackIndex < 0 || trackIndex > 11) trackIndex = 11;

    sprintf(buffer, "Musique: < %s >", musicNames[trackIndex]);
    renderText(app, buffer, midX, startY + spacing * 4, (game->menuSelectedOption == 4) ? highlight : white, app->fontLarge, 1);

    renderText(app, "Configurer Touches >", midX, startY + spacing * 5, (game->menuSelectedOption == 5) ? highlight : white, app->fontLarge, 1);

    char* resText = "800x700";
    if (game->menuResolution == 1) resText = "1200x1050";
    else if (game->menuResolution == 2) resText = "Plein Ecran";
    sprintf(buffer, "Resolution: < %s >", resText);
    renderText(app, buffer, midX, startY + spacing * 6, (game->menuSelectedOption == 6) ? highlight : white, app->fontLarge, 1);

    renderText(app, "Reinitialiser", midX, startY + spacing * 7, (game->menuSelectedOption == 7) ? highlight : white, app->fontLarge, 1);
    renderText(app, "Retour", midX, startY + spacing * 8, (game->menuSelectedOption == 8) ? highlight : white, app->fontLarge, 1);

    SDL_RenderPresent(app->renderer);
}

void renderMenu(AppContext* app, GameContext* game) {
    if (game->state == STATE_KEY_CONFIG) { renderKeyConfigMenu(app, game); return; }
    if (game->state == STATE_SETTINGS) { renderSettingsMenu(app, game); return; }

    SDL_SetRenderDrawColor(app->renderer, 16, 16, 26, 255);
    SDL_RenderClear(app->renderer);

    if (app->menuBackground) SDL_RenderCopy(app->renderer, app->menuBackground, NULL, NULL);

    // Décoration
    renderPiecePreview(app, 0, 50, 150);
    renderPiecePreview(app, 1, 650, 150);
    renderPiecePreview(app, 3, 620, 300);
    renderPiecePreview(app, 2, 80, 300);
    renderPiecePreview(app, 5, 50, 600);
    renderPiecePreview(app, 4, 650, 450);
    renderPiecePreview(app, 6, 50, 450);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color highlight = {255, 255, 0, 255};
    SDL_Color recordColor = {0, 255, 255, 255};

    int midX = LOGICAL_WIDTH / 2;
    int startY = 200;
    int spacing = 50;
    char buffer[64];

    // Stats High Score
    sprintf(buffer, "High Score: %d", game->highScore);
    renderText(app, buffer, 20, 20, recordColor, app->fontLarge, 0);

    sprintf(buffer, "Best Lines: %d", game->bestLines);
    renderText(app, buffer, LOGICAL_WIDTH - 250, 20, recordColor, app->fontLarge, 0);

    renderText(app, "TETRIS", midX, 100, highlight, app->fontLarge, 1);

    int offset = 0;
    if (game->gameInProgress) offset = 1;

    // Logique Menu "Continuer" vs "Commencer"
    if (game->gameInProgress) {
        renderText(app, "Continuer", midX, startY, (game->menuSelectedOption == 0) ? highlight : white, app->fontLarge, 1);
        renderText(app, "Recommencer", midX, startY + spacing, (game->menuSelectedOption == 1) ? highlight : white, app->fontLarge, 1);
    } else {
        renderText(app, "Commencer", midX, startY, (game->menuSelectedOption == 0) ? highlight : white, app->fontLarge, 1);
    }

    renderText(app, "Parametres", midX, startY + spacing * (1 + offset), (game->menuSelectedOption == 1 + offset) ? highlight : white, app->fontLarge, 1);
    renderText(app, "Quitter", midX, startY + spacing * (2 + offset), (game->menuSelectedOption == 2 + offset) ? highlight : white, app->fontLarge, 1);

    SDL_RenderPresent(app->renderer);
}