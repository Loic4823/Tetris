#ifndef DEFS_H
#define DEFS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#define LOGICAL_WIDTH 800
#define LOGICAL_HEIGHT 700
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 30

#define BOARD_X_OFFSET 140 
#define BOARD_Y_OFFSET 50

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
    
    // NOUVEAU : Tableau pour les 10 musiques
    Mix_Music* musics[10]; 
    
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
    
    // Système 7-Bag
    int pieceBag[7]; 
    int bagIndex;    

    int menuSelectedOption;
    int menuAutoSpeed;
    int menuStartLevel;
    int menuTextureStyle; 
    int menuMusicTrack; // 0 à 9 = Musiques, 10 = Aucune
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
} GameContext;

#endif