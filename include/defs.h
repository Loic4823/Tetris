#ifndef DEFS_H
#define DEFS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdbool.h>

// Dimensions logiques (indépendantes de la résolution réelle de la fenêtre)
#define LOGICAL_WIDTH 800
#define LOGICAL_HEIGHT 700

// Dimensions de la grille de jeu
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIZE 30

// Position de la grille sur l'écran
#define BOARD_X_OFFSET 140
#define BOARD_Y_OFFSET 50

// Temps (ms) avant qu'une pièce touchant le sol ne se verrouille définitivement
#define LOCK_DELAY 500

// --- [IMPORTANT] CONSTANTES DE JEU OFFICIELLES (Mécaniques Tetris) ---
// DAS (Delayed Auto Shift) : Temps d'attente avant que la pièce ne glisse vite quand on maintient une touche.
#define DAS_DELAY 170 
// ARR (Auto Repeat Rate) : Vitesse de déplacement une fois le DAS activé.
#define ARR_RATE 50   
// Limite le nombre de fois qu'on peut bouger une pièce au sol pour empêcher de jouer indéfiniment (Infinite Spin).
#define MAX_LOCK_RESETS 15 
// -------------------------------------

// Les différents écrans du jeu
typedef enum {
    STATE_MENU,
    STATE_SETTINGS,
    STATE_KEY_CONFIG,
    STATE_PLAYING,     // Le jeu est en cours
    STATE_ANIMATING,   // Animation de suppression de lignes
    STATE_GAMEOVER,
    STATE_QUIT
} GameState;

// Représente une pièce active
typedef struct {
    int x;          // Position grille X
    int y;          // Position grille Y
    int type;       // Type de tétromino (I, O, T, S, Z, J, L)
    int rotation;   // Orientation (0, 1, 2, 3)
} Piece;

// [IMPORTANT] Contient toutes les ressources lourdes (Assets)
// Séparé du GameContext pour ne pas recharger les textures à chaque nouvelle partie.
typedef struct {
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Texture* blockTextures[7]; // Textures des blocs normaux
    SDL_Texture* ghostTexture;     // Texture de la "pièce fantôme" (projection au sol)
    
    SDL_Texture* menuBackground;
    SDL_Texture* settingsBackground;

    TTF_Font* fontLarge;
    TTF_Font* fontSmall;
    
    Mix_Music* musics[12]; // Playlist
    Mix_Chunk* soundClear; // Bruitage ligne complétée
} AppContext;

// [IMPORTANT] Contient TOUTE la logique et les données d'une partie en cours.
typedef struct {
    // La grille : stocke les ID des couleurs/types, 0 si vide.
    int board[BOARD_HEIGHT][BOARD_WIDTH];
    
    Piece currentPiece;  // La pièce qu'on contrôle
    int nextPieceType;   // La prochaine pièce (preview)
    int heldPieceType;   // La pièce en réserve (Hold)
    int canHold;         // Empêche d'utiliser le Hold deux fois de suite sans poser la pièce
    
    int score;
    int linesCleared;
    int level;           // Niveau actuel (accélère la chute)
    int fallInterval;    // Vitesse de chute actuelle en ms
    
    // --- TIMERS ---
    int fallTimer;       // Compteur pour la gravité automatique
    int lockTimer;       // Compteur pour le verrouillage au sol (Lock Delay)
    int lockDelayResets; // Compteur de mouvements au sol (anti-triche Infinite Spin)
    
    int lowestY;         // Plus bas point atteint par la pièce actuelle (pour le Lock Delay)
    
    // --- RANDOMIZER (Système de "Sac") ---
    // Tetris utilise un "sac" de 7 pièces mélangées pour garantir une distribution équitable.
    int pieceBag[7]; 
    int bagIndex;

    // --- OPTIONS DU MENU ---
    int menuSelectedOption;
    int menuAutoSpeed;
    int menuStartLevel;
    int menuTextureStyle;
    int menuMusicTrack;
    int menuResolution;
    
    // Drapeaux pour demander des actions au moteur principal
    int reloadAssetsPending;
    int changeMusicPending;
    int changeResolutionPending;
    int playSoundClearPending;

    // Configuration des touches (Rebinding)
    int keyConfigSelection;
    int keyConfigColumn;
    int isRebinding;

    int isPaused;
    int gameInProgress;
    GameState state;

    // Statistiques globales
    int highScore;
    int bestLines;

    int masterVolume;
    int isMuted;

    // Animation de ligne
    int animTimer;
    int linesToClear[4];    // Indices des lignes à supprimer
    int linesToClearCount;

    // --- [CRITIQUE] GESTION DES INPUTS FLUIDES (DAS/ARR) ---
    int dasDirection; // -1 (Gauche), 1 (Droite), 0 (Rien)
    int dasTimer;     // Chronomètre pour déclencher le mouvement rapide
    int dasPhase;     // 0 = Initial, 1 = Attente DAS, 2 = Mode Rapide (ARR)

    // --- T-SPIN & MESSAGES ---
    int lastActionWasRotate; // Nécessaire pour valider un T-Spin (doit être la dernière action)
    int tSpinBonus;          // Détecte si la manœuvre est technique (plus de points)
    
    char messageText[32];    // Texte flottant ("TETRIS", "T-SPIN DOUBLE")
    int messageTimer;        
    
} GameContext;

#endif