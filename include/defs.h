#ifndef DEFS_H // Si "DEFS_H" n'est pas encore défini...
#define DEFS_H // ...alors on le définit. Ça évite que ce fichier soit inclus 2 fois (ce qui ferait planter la compilation).

#include <SDL.h>       // On inclut SDL pour gérer la fenêtre et le rendu graphique.
#include <SDL_ttf.h>   // On inclut SDL_ttf pour gérer les polices d'écriture (texte).
#include <SDL_mixer.h> // On inclut SDL_mixer pour gérer le son (musique et bruitages).

#define LOGICAL_WIDTH 800  // On définit la largeur virtuelle de la fenêtre (le jeu pensera toujours faire cette taille).
#define LOGICAL_HEIGHT 700 // On définit la hauteur virtuelle.
#define BOARD_WIDTH 10     // Le plateau de Tetris fait toujours 10 cases de large.
#define BOARD_HEIGHT 20    // Le plateau fait toujours 20 cases de haut.
#define BLOCK_SIZE 30      // Chaque carré (bloc) fera 30x30 pixels à l'écran.

#define BOARD_X_OFFSET 140 // On décale le plateau de 140px vers la droite pour le centrer un peu.
#define BOARD_Y_OFFSET 50  // On le décale de 50px vers le bas pour laisser de la place au titre.

#define LOCK_DELAY 500 // Le temps (en ms) qu'on laisse au joueur pour bouger la pièce une fois qu'elle touche le sol.

// On liste tous les états possibles du jeu. C'est une machine à états.
typedef enum {
    STATE_MENU,       // On est dans le menu principal.
    STATE_SETTINGS,   // On est dans les paramètres.
    STATE_KEY_CONFIG, // On est en train de changer les touches.
    STATE_PLAYING,    // Le jeu est en cours.
    STATE_ANIMATING,  // Une ligne vient d'être faite, on joue l'animation.
    STATE_GAMEOVER,   // Le joueur a perdu.
    STATE_QUIT        // Le joueur veut quitter le programme.
} GameState;

// Une structure simple pour représenter un bloc ou une pièce.
typedef struct {
    int x;        // Sa position horizontale dans la grille (0 à 9).
    int y;        // Sa position verticale dans la grille (0 à 19).
    int type;     // Quel type de pièce c'est ? (I, O, T, S, Z, J, L).
    int rotation; // Dans quel sens elle est tournée (0, 1, 2, 3).
} Piece;

// Cette structure contient tout ce qui concerne l'affichage et les fichiers (Assets).
typedef struct {
    SDL_Renderer* renderer; // C'est le "peintre" de la carte graphique qui dessine à l'écran.
    SDL_Window* window;     // C'est la fenêtre physique du système (Windows/Linux).
    SDL_Texture* blockTextures[7]; // Un tableau pour stocker les images des 7 blocs de couleur.
    SDL_Texture* ghostTexture;     // L'image transparente pour le fantôme de la pièce.
    
    SDL_Texture* menuBackground;     // L'image de fond du menu principal.
    SDL_Texture* settingsBackground; // L'image de fond des paramètres.

    TTF_Font* fontLarge; // La grosse police pour les titres.
    TTF_Font* fontSmall; // La petite police pour les infos.
    
    Mix_Music* musics[11]; // Un tableau pour stocker toutes les musiques (0 à 10).
    
    Mix_Chunk* soundClear; // Le bruitage court quand on fait une ligne ("Chunk" = petit son).
} AppContext;

// Cette structure contient TOUTES les données de la partie en cours (le "State").
// C'est ça qu'on sauvegarde dans le fichier save.dat.
typedef struct {
    int board[BOARD_HEIGHT][BOARD_WIDTH]; // La grille de jeu : ce qui est vide (-1) et ce qui est rempli (couleur).
    Piece currentPiece; // La pièce qui est en train de tomber.
    int nextPieceType;  // Le type de la prochaine pièce à venir.
    int heldPieceType;  // Le type de la pièce mise en réserve (Hold).
    int canHold;        // Est-ce qu'on a le droit d'utiliser le Hold ? (1 fois par tour).
    int score;          // Le score actuel.
    int linesCleared;   // Nombre de lignes détruites.
    int level;          // Le niveau actuel (augmente la vitesse).
    int fallInterval;   // Le temps en ms entre deux chutes automatiques (vitesse).
    
    int fallTimer; // Chrono interne pour savoir quand faire tomber la pièce d'un cran.
    int lockTimer; // Chrono pour savoir combien de temps la pièce reste au sol avant de se figer.
    
    int lowestY; // La position la plus basse atteinte par la pièce (pour la règle du "Step Reset").
    
    int pieceBag[7]; // Le "sac" de 7 pièces pour le tirage aléatoire équitable.
    int bagIndex;    // Où on en est dans le sac (0 à 6).

    int menuSelectedOption; // Sur quel bouton le curseur est positionné dans le menu.
    int menuAutoSpeed;      // Option : est-ce que la vitesse augmente toute seule ?
    int menuStartLevel;     // Option : niveau de départ choisi.
    
    int menuTextureStyle; // 0=Défaut, 1=Alt, 2=MC (Minecraft).
    
    int menuMusicTrack; // Quelle musique est choisie ?
    int menuResolution; // Quelle taille d'écran ?
    
    // Des drapeaux (flags) pour dire au moteur graphique de recharger des trucs au prochain tour.
    int reloadAssetsPending;     // "Hé, faut recharger les images !"
    int changeMusicPending;      // "Hé, faut changer la musique !"
    int changeResolutionPending; // "Hé, change la taille de la fenêtre !"
    int playSoundClearPending;   // "Hé, joue le son de ligne !"

    int keyConfigSelection; // Quelle action on configure dans le menu touches.
    int keyConfigColumn;    // Quelle colonne (touche 1 ou 2) on modifie.
    int isRebinding;        // Est-ce qu'on est en attente d'une pression de touche ?

    int isPaused;       // Le jeu est-il en pause ?
    int gameInProgress; // Y a-t-il une partie commencée ? (pour afficher "Continuer").
    GameState state;    // Dans quel écran on est (Jeu, Menu, etc.).

    int highScore; // Le meilleur score enregistré.
    int bestLines; // Le meilleur nombre de lignes.

    int masterVolume; // Le volume global (0-128).
    int isMuted;      // Est-ce que le son est coupé ?

    int animTimer;         // Chrono pour l'animation de destruction de ligne.
    int linesToClear[4];   // La liste des lignes à supprimer (max 4 d'un coup).
    int linesToClearCount; // Combien de lignes on supprime ce tour-ci.
} GameContext;

#endif // Fin du DEFS_H