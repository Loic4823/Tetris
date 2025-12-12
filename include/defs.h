#ifndef DEFS_H
#define DEFS_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdbool.h> 

#define LOGICAL_WIDTH 800
#define LOGICAL_HEIGHT 700
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 30
#define BOARD_X_OFFSET 50
#define BOARD_Y_OFFSET 50

#define DAS_DELAY 170
#define ARR_RATE 50
#define MAX_LOCK_RESETS 5 

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_SETTINGS,
    STATE_KEY_CONFIG,
    STATE_ANIMATING 
} GameState;

typedef enum {
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_DOWN,
    ACTION_UP,
    ACTION_A, 
    ACTION_E, 
    ACTION_C, 
    ACTION_COUNT 
} GameAction;

typedef struct {
    SDL_Keycode keyPrimary;
    SDL_Keycode keySecondary;
} KeyBinding;

typedef struct {
    int x, y;
    int type;
    int rotation;
} Tetromino;

typedef struct {
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    Tetromino currentPiece;
    int nextPieceType;
    int heldPieceType;     
    int canHold;           
    int score;
    int linesCleared;
    int level;
    int speed;
    Uint32 lastDropTime;
    GameState state;
    
    int moveDirection;
    int moveTimer;
    int lockDelayResets; 
    
    int isMuted;       
    int masterVolume;  
    
    int isPaused;
    int highScore;
    int bestLines; 

    int menuSelectedOption;
    int menuTextureStyle; 
    int menuAutoSpeed;    
    int menuStartLevel;   
    int menuMusicTrack;   
    int menuResolution;   
    int gameInProgress;   

    int keyConfigSelection; 
    int keyConfigColumn;    
    int isRebinding;        

    int linesToClear[4];    
    int linesToClearCount;  
    Uint32 animStartTime;   
    float animTimer;        
} GameContext;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* blockTextures[7];
    SDL_Texture* ghostTexture;
    SDL_Texture* menuBackground;
    SDL_Texture* settingsBackground;
    TTF_Font* fontLarge;
    TTF_Font* fontSmall;
    Mix_Music* musics[11]; 
    Mix_Chunk* soundClear;
} AppContext;

#endif