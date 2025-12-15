#ifndef DEFS_H
#define DEFS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdbool.h>

#define LOGICAL_WIDTH 800
#define LOGICAL_HEIGHT 700
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 30

#define BOARD_X_OFFSET 140
#define BOARD_Y_OFFSET 50

#define LOCK_DELAY 500

// --- CONSTANTES DE JEU OFFICIELLES ---
#define DAS_DELAY 170 
#define ARR_RATE 50   
#define MAX_LOCK_RESETS 15 // Limite officielle de mouvements au sol
// -------------------------------------

typedef enum {
    STATE_MENU,
    STATE_SETTINGS,
    STATE_KEY_CONFIG,
    STATE_PLAYING,
    STATE_ANIMATING,
    STATE_GAMEOVER,
    STATE_QUIT
} GameState;

typedef struct {
    int x;
    int y;
    int type;
    int rotation;
} Piece;

typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Texture* blockTextures[7];
    SDL_Texture* ghostTexture;
    
    SDL_Texture* menuBackground;
    SDL_Texture* settingsBackground;

    TTF_Font* fontLarge;
    TTF_Font* fontSmall;
    
    Mix_Music* musics[12];
    
    Mix_Chunk* soundClear;
} AppContext;

typedef struct {
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    Piece currentPiece;
    int nextPieceType;
    int heldPieceType;
    int canHold;
    int score;
    int linesCleared;
    int level;
    int fallInterval;
    
    int fallTimer;
    int lockTimer;
    int lockDelayResets; 
    
    int lowestY;
    
    int pieceBag[7];
    int bagIndex;

    int menuSelectedOption;
    int menuAutoSpeed;
    int menuStartLevel;
    
    int menuTextureStyle;
    
    int menuMusicTrack;
    int menuResolution;
    
    int reloadAssetsPending;
    int changeMusicPending;
    int changeResolutionPending;
    int playSoundClearPending;

    int keyConfigSelection;
    int keyConfigColumn;
    int isRebinding;

    int isPaused;
    int gameInProgress;
    GameState state;

    int highScore;
    int bestLines;

    int masterVolume;
    int isMuted;

    int animTimer;
    int linesToClear[4];
    int linesToClearCount;

    // Variables DAS/ARR
    int dasDirection; 
    int dasTimer;     
    int dasPhase;     

    // --- T-SPIN & MESSAGES (NOUVEAU) ---
    int lastActionWasRotate; // 1 si la dernière action était une rotation
    int tSpinBonus;          // 0 = Non, 1 = T-Spin détecté
    
    char messageText[32];    // Pour afficher "TETRIS", "T-SPIN", etc.
    int messageTimer;        // Temps d'affichage du message
    // -----------------------------------
    
} GameContext;

#endif