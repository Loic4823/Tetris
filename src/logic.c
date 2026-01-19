#include "logic.h"
#include "audio.h"
#include "input.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

// --- DEFINITION DES PIECES (SRS OFFICIEL) ---
// Tableau 4D : [Type][Rotation][Bloc 0-3][Coordonnées Y,X]
const int TETROMINO_SHAPES[7][4][4][2] = {
    // ... (Données des formes I, O, T, S, Z, J, L) ...
    // J'abrège ici pour la lisibilité, le contenu est le même que ton fichier original
    // I (Type 0)
    { {{1,0}, {1,1}, {1,2}, {1,3}}, {{0,2}, {1,2}, {2,2}, {3,2}}, {{2,0}, {2,1}, {2,2}, {2,3}}, {{0,1}, {1,1}, {2,1}, {3,1}} },
    // O (Type 1)
    { {{0,1}, {0,2}, {1,1}, {1,2}}, {{0,1}, {0,2}, {1,1}, {1,2}}, {{0,1}, {0,2}, {1,1}, {1,2}}, {{0,1}, {0,2}, {1,1}, {1,2}} },
    // T (Type 2)
    { {{0,1}, {1,0}, {1,1}, {1,2}}, {{0,1}, {1,1}, {2,1}, {1,2}}, {{1,0}, {1,1}, {1,2}, {2,1}}, {{0,1}, {1,1}, {2,1}, {1,0}} },
    // S (Type 3)
    { {{0,1}, {0,2}, {1,0}, {1,1}}, {{0,1}, {1,1}, {1,2}, {2,2}}, {{1,1}, {1,2}, {2,0}, {2,1}}, {{0,0}, {1,0}, {1,1}, {2,1}} },
    // Z (Type 4)
    { {{0,0}, {0,1}, {1,1}, {1,2}}, {{0,2}, {1,1}, {1,2}, {2,1}}, {{1,0}, {1,1}, {2,1}, {2,2}}, {{0,1}, {1,0}, {1,1}, {2,0}} },
    // J (Type 5)
    { {{0,0}, {1,0}, {1,1}, {1,2}}, {{0,1}, {0,2}, {1,1}, {2,1}}, {{1,0}, {1,1}, {1,2}, {2,2}}, {{0,1}, {1,1}, {2,1}, {2,0}} },
    // L (Type 6)
    { {{0,2}, {1,0}, {1,1}, {1,2}}, {{0,1}, {1,1}, {2,1}, {2,2}}, {{1,0}, {1,1}, {1,2}, {2,0}}, {{0,0}, {0,1}, {1,1}, {2,1}} }
};

// --- TABLES DE WALL KICKS SRS ---
// [IMPORTANT] Ces tables définissent comment la pièce se décale si elle tourne contre un mur.
// C'est ce qui permet de faire des rotations "impossibles" (T-Spin techniques).
const int WALL_KICKS_JLSTZ[8][5][2] = {
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // 0 -> 1 (R)
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},     // 1 -> 0 (L)
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},     // 1 -> 2 (R)
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}}, // 2 -> 1 (L)
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},    // 2 -> 3 (R)
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},  // 3 -> 2 (L)
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},  // 3 -> 0 (R)
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}     // 0 -> 3 (L)
};

const int WALL_KICKS_I[8][5][2] = {
    // La barre (I) a ses propres règles de rotation car elle est longue.
    {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},   
    {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},   
    {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},   
    {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},   
    {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},   
    {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},   
    {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},   
    {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}    
};

// --- 7-BAG (RANDOMIZER EQUITABLE) ---
// Au lieu d'un pur hasard (qui pourrait donner 3 carrés de suite),
// on met les 7 formes dans un sac, on mélange, on distribue, et on recommence.
static void shuffleBag(GameContext* game) {
    for (int i = 0; i < 7; i++) {
        game->pieceBag[i] = i;
    }
    for (int i = 6; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = game->pieceBag[i];
        game->pieceBag[i] = game->pieceBag[j];
        game->pieceBag[j] = temp;
    }
    game->bagIndex = 0;
}

int getRandomPieceType(GameContext* game) {
    if (game->bagIndex >= 7) {
        shuffleBag(game);
    }
    return game->pieceBag[game->bagIndex++];
}

// --- SAUVEGARDE ET CHARGEMENT ---
void saveGameData(GameContext* game, int saveFullState) {
    FILE* f = fopen("save.dat", "wb");
    if (f) {
        fwrite(&saveFullState, sizeof(int), 1, f);
        fwrite(game, sizeof(GameContext), 1, f);
        SaveInputProfile(f); // On sauvegarde aussi les touches
        fclose(f);
    }
}

void loadGameData(GameContext* game) {
    FILE* f = fopen("save.dat", "rb");
    if (f) {
        int hasSave = 0;
        if (fread(&hasSave, sizeof(int), 1, f) == 1) {
            GameContext tempGame;
            if (fread(&tempGame, sizeof(GameContext), 1, f) == 1) {
                *game = tempGame;
                // Si une partie était en cours, on la met en pause au démarrage
                if (hasSave) {
                    game->state = STATE_MENU;
                    game->isPaused = 1;
                    game->gameInProgress = 1;
                } else {
                    game->gameInProgress = 0;
                    game->state = STATE_MENU;
                }
                // On force le rechargement des assets car les pointeurs SDL ne sont pas sauvegardés
                game->reloadAssetsPending = 1;
                game->changeMusicPending = 1;
                game->changeResolutionPending = 1;
            }
        }
        LoadInputProfile(f);
        fclose(f);
    } else {
        // Pas de sauvegarde : valeurs par défaut
        game->highScore = 0;
        game->bestLines = 0;
        game->gameInProgress = 0;
    }
}

void checkHighScore(GameContext* game) {
    if (game->score > game->highScore) game->highScore = game->score;
    if (game->linesCleared > game->bestLines) game->bestLines = game->linesCleared;
}

void initDefaultKeys(GameContext* game) {}

void initGame(GameContext* game) {
    srand(time(NULL));

    // Valeurs par défaut du menu
    game->state = STATE_MENU;
    game->menuSelectedOption = 0;
    game->menuAutoSpeed = 1;
    game->menuStartLevel = 1;
    game->menuTextureStyle = 0;
    game->menuMusicTrack = 0;
    game->menuResolution = 0;

    game->reloadAssetsPending = 0;
    game->changeMusicPending = 1;
    game->changeResolutionPending = 0;
    game->playSoundClearPending = 0;

    game->keyConfigSelection = 0;
    game->keyConfigColumn = 0;
    game->isRebinding = 0;

    game->masterVolume = 0;
    game->isMuted = 1;

    game->animTimer = 0;
    game->linesToClearCount = 0;
    game->gameInProgress = 0;

    game->bagIndex = 7;
    game->fallTimer = 0;
    game->lockTimer = 0;
    game->lockDelayResets = 0;
    
    // Initialisation T-Spin
    game->lastActionWasRotate = 0;
    game->tSpinBonus = 0;
    game->messageTimer = 0;
    sprintf(game->messageText, "");

    game->nextPieceType = getRandomPieceType(game);
    game->heldPieceType = -1;
    game->lowestY = -100;

    game->dasDirection = 0;
    game->dasTimer = 0;
    game->dasPhase = 0;

    initDefaultKeys(game);
    loadGameData(game);

    game->changeResolutionPending = 1;
    game->reloadAssetsPending = 1;
    game->changeMusicPending = 1;
}

void resetSettings(GameContext* game) {
    game->menuTextureStyle = 0;
    game->menuAutoSpeed = 1;
    game->menuStartLevel = 1;
    game->menuMusicTrack = 0;
    game->menuResolution = 0;

    audioSystem.volume = 0.0f;
    audioSystem.isMuted = true;
    audioSystem.previousVolume = 0.5f;
    ApplyVolume(); 
    game->masterVolume = 0; 
    game->isMuted = 1;

    game->reloadAssetsPending = 1;
    game->changeMusicPending = 1;
    game->changeResolutionPending = 1;

    InitInputProfile();
}

// [COLLISIONS] Vérifie si la pièce à (px, py) touche un mur ou un bloc existant
static int checkCollision(GameContext* game, int px, int py, int type, int rot) {
    for (int i = 0; i < 4; i++) {
        int bx = px + TETROMINO_SHAPES[type][rot][i][1];
        int by = py + TETROMINO_SHAPES[type][rot][i][0];
        
        // Sortie de grille (Gauche, Droite, Bas)
        if (bx < 0 || bx >= BOARD_WIDTH || by >= BOARD_HEIGHT) return 1;
        
        // Collision avec un bloc déjà posé (seulement si y >= 0)
        if (by >= 0 && game->board[by][bx] != -1) return 1;
    }
    return 0;
}

static int isOccupied(GameContext* game, int x, int y) {
    if (x < 0 || x >= BOARD_WIDTH || y >= BOARD_HEIGHT) return 1; // Mur = Occupé
    if (y < 0) return 0; // Au dessus de l'écran = Vide
    return (game->board[y][x] != -1);
}

// [T-SPIN] Détecte la manoeuvre technique T-Spin
static int checkTSpin(GameContext* game) {
    if (game->currentPiece.type != 2) return 0; // Doit être un 'T'
    if (!game->lastActionWasRotate) return 0;   // La dernière action DOIT être une rotation

    int x = game->currentPiece.x;
    int y = game->currentPiece.y;
    int corners = 0;

    // On vérifie les 4 coins autour du centre du T
    if (isOccupied(game, x, y)) corners++;         // Haut-Gauche
    if (isOccupied(game, x + 2, y)) corners++;     // Haut-Droite
    if (isOccupied(game, x, y + 2)) corners++;     // Bas-Gauche
    if (isOccupied(game, x + 2, y + 2)) corners++; // Bas-Droite

    // Règle officielle : 3 coins occupés = T-Spin
    return (corners >= 3);
}

static void spawnPiece(GameContext* game) {
    game->currentPiece.type = game->nextPieceType;
    game->currentPiece.rotation = 0;
    game->currentPiece.x = BOARD_WIDTH / 2 - 2;
    game->currentPiece.y = 0;
    game->nextPieceType = getRandomPieceType(game);
    game->canHold = 1;
    game->fallTimer = 0;
    game->lockTimer = 0;
    game->lowestY = game->currentPiece.y;
    game->lockDelayResets = 0;
    game->dasDirection = 0;
    
    // Reset T-Spin status
    game->lastActionWasRotate = 0;
    game->tSpinBonus = 0;

    // Game Over instantané si la nouvelle pièce touche quelque chose
    if (checkCollision(game, game->currentPiece.x, game->currentPiece.y, game->currentPiece.type, 0)) {
        game->state = STATE_GAMEOVER;
        game->gameInProgress = 0;
        checkHighScore(game);
        saveGameData(game, 0);
    }
}

void resetGameLogic(GameContext* game) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game->board[y][x] = -1;
        }
    }
    game->score = 0;
    game->linesCleared = 0;
    game->level = game->menuStartLevel;
    game->isPaused = 0;
    game->heldPieceType = -1;
    game->canHold = 1;
    game->gameInProgress = 1;
    game->bagIndex = 7;
    
    game->messageTimer = 0;

    // Calcul vitesse : (0.8 - ((Level-1)*0.007))^ (Level-1) - Formule approximative Tetris Worlds
    double multiplier = pow(0.75, game->level - 1);
    game->fallInterval = (int)(multiplier * 1000);
    if (game->fallInterval < 50) game->fallInterval = 50; // Cap vitesse max

    game->dasDirection = 0;
    game->dasTimer = 0;

    game->nextPieceType = getRandomPieceType(game);
    spawnPiece(game);
    game->fallTimer = 0;
    game->lockTimer = 0;
    game->state = STATE_PLAYING;
}

static void performLineClear(GameContext* game) {
    int linesFound = game->linesToClearCount;
    
    // Suppression des lignes et descente des blocs du dessus
    for (int i = 0; i < linesFound; i++) {
        int y = game->linesToClear[i];
        for (int row = y; row > 0; row--) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                game->board[row][x] = game->board[row - 1][x];
            }
        }
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game->board[0][x] = -1;
        }
    }

    // Gestion du SCORING
    if (linesFound > 0) {
        game->linesCleared += linesFound;
        
        int baseScore = 0;
        
        if (game->tSpinBonus) {
            // T-Spin Bonus !
            switch(linesFound) {
                case 1: baseScore = 800; sprintf(game->messageText, "T-SPIN SINGLE"); break;
                case 2: baseScore = 1200; sprintf(game->messageText, "T-SPIN DOUBLE"); break;
                case 3: baseScore = 1600; sprintf(game->messageText, "T-SPIN TRIPLE"); break;
                default: baseScore = 400; sprintf(game->messageText, "T-SPIN"); break;
            }
        } else {
            // Scoring Standard
            switch(linesFound) {
                case 1: baseScore = 100; break;
                case 2: baseScore = 300; break;
                case 3: baseScore = 500; break;
                case 4: baseScore = 800; sprintf(game->messageText, "TETRIS !"); break;
            }
        }
        
        if (baseScore > 0) {
            game->score += baseScore * game->level;
            if (game->tSpinBonus || linesFound == 4) {
                game->messageTimer = 120; // Afficher le message pendant 2 sec
            }
        }
        
        // Augmentation du niveau
        int newLevel = game->menuStartLevel + game->linesCleared / 10;
        if (newLevel > 10) newLevel = 10;
        if (newLevel > game->level) {
            game->level = newLevel;
            if (game->menuAutoSpeed) {
                double multiplier = pow(0.75, game->level - 1);
                game->fallInterval = (int)(multiplier * 1000);
                if (game->fallInterval < 50) game->fallInterval = 50;
            }
        }
    } else {
        // T-Spin Zero (Rare : rotation T-Spin sans faire de ligne)
        if (game->tSpinBonus) {
             game->score += 400 * game->level;
             sprintf(game->messageText, "T-SPIN");
             game->messageTimer = 80;
        }
    }
    
    game->linesToClearCount = 0;
    game->animTimer = 0;
    game->state = STATE_PLAYING;
    spawnPiece(game);
}

static void detectLines(GameContext* game) {
    game->linesToClearCount = 0;
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        int full = 1;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x] == -1) {
                full = 0;
                break;
            }
        }
        if (full) game->linesToClear[game->linesToClearCount++] = y;
    }
    if (game->linesToClearCount > 0) {
        game->state = STATE_ANIMATING; // Passe en mode animation
        game->animTimer = 0;
        game->playSoundClearPending = 1;
    } else {
        performLineClear(game); 
    }
}

static void lockPiece(GameContext* game) {
    // 1. Détecter le T-Spin AVANT de verrouiller la pièce dans la grille
    if (checkTSpin(game)) {
        game->tSpinBonus = 1;
    } else {
        game->tSpinBonus = 0;
    }

    // Copie de la pièce active vers la grille statique
    for (int i = 0; i < 4; i++) {
        int bx = game->currentPiece.x + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][1];
        int by = game->currentPiece.y + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][0];
        if (by >= 0 && by < BOARD_HEIGHT && bx >= 0 && bx < BOARD_WIDTH) {
            game->board[by][bx] = game->currentPiece.type;
        } else if (by < 0) {
            // Lock Out (Game Over si pièce bloquée en haut)
            game->state = STATE_GAMEOVER;
            game->gameInProgress = 0;
            checkHighScore(game);
            saveGameData(game, 0);
            return;
        }
    }
    detectLines(game);
}

// Empêche le "Infinite Spin" en limitant le nombre de reset du timer au sol
static void resetLockTimerIfAllowed(GameContext* game) {
    if (game->lockDelayResets < MAX_LOCK_RESETS) {
        game->lockTimer = 0;
        game->lockDelayResets++;
    }
}

static void movePiece(GameContext* game, int dx, int dy) {
    if (game->state != STATE_PLAYING || game->isPaused) return;
    if (!checkCollision(game, game->currentPiece.x + dx, game->currentPiece.y + dy, game->currentPiece.type, game->currentPiece.rotation)) {
        game->currentPiece.x += dx;
        game->currentPiece.y += dy;
        
        game->lastActionWasRotate = 0; // Le mouvement annule le T-Spin
        
        if (dy > 0) {
            // Si on tombe
            game->fallTimer = 0;
            if (game->currentPiece.y > game->lowestY) {
                game->lowestY = game->currentPiece.y;
                game->lockTimer = 0;      // Reset le temps avant verrouillage
                game->lockDelayResets = 0; // Reset le compteur anti-triche
            }
        } else {
            // Si on bouge latéralement au sol
            if (checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.type, game->currentPiece.rotation)) {
                resetLockTimerIfAllowed(game);
            }
        }
    }
}

// [ROTATION SRS] Cœur du système de rotation complexe
static void rotatePiece(GameContext* game, int dir) {
    if (game->state != STATE_PLAYING || game->isPaused) return;
    if (game->currentPiece.type == 1) return; // O ne tourne pas

    int oldRot = game->currentPiece.rotation;
    int newRot = (oldRot + dir + 4) % 4; // Modulo pour rester entre 0 et 3
    int pieceType = game->currentPiece.type;

    // Détermination de la table de Wall Kicks à utiliser
    int kickIndex = 0;
    // ... (Logique de sélection de l'index selon oldRot et newRot) ...
    if      (oldRot == 0 && newRot == 1) kickIndex = 0;
    else if (oldRot == 1 && newRot == 0) kickIndex = 1;
    else if (oldRot == 1 && newRot == 2) kickIndex = 2;
    else if (oldRot == 2 && newRot == 1) kickIndex = 3;
    else if (oldRot == 2 && newRot == 3) kickIndex = 4;
    else if (oldRot == 3 && newRot == 2) kickIndex = 5;
    else if (oldRot == 3 && newRot == 0) kickIndex = 6;
    else if (oldRot == 0 && newRot == 3) kickIndex = 7;

    const int (*currentKicks)[2] = (pieceType == 0) ? WALL_KICKS_I[kickIndex] : WALL_KICKS_JLSTZ[kickIndex];

    // On teste 5 positions (la rotation normale + 4 décalages "kicks")
    for (int i = 0; i < 5; i++) {
        int kickX = currentKicks[i][0];
        int kickY = currentKicks[i][1];

        int testX = game->currentPiece.x + kickX;
        int testY = game->currentPiece.y - kickY; // Y inversé car SDL Y augmente vers le bas

        if (!checkCollision(game, testX, testY, pieceType, newRot)) {
            // Rotation Validée !
            game->currentPiece.x = testX;
            game->currentPiece.y = testY;
            game->currentPiece.rotation = newRot;
            
            game->lastActionWasRotate = 1; // Marque cruciale pour le T-Spin

            // Si on tourne alors qu'on est au sol, on reset le timer de verrouillage (permet d'ajuster)
            if (checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, pieceType, newRot)) {
                resetLockTimerIfAllowed(game);
            }
            return; 
        }
    }
}

static void dropPiece(GameContext* game) {
    if (game->state != STATE_PLAYING || game->isPaused) return;
    // Hard Drop : boucle jusqu'à toucher le fond
    while (!checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.type, game->currentPiece.rotation)) {
        game->currentPiece.y++;
        game->score += 2; // Bonus de points pour drop rapide
        game->lastActionWasRotate = 0;
    }
    lockPiece(game);
}

static void holdPiece(GameContext* game) {
    if (game->state != STATE_PLAYING || game->isPaused || !game->canHold) return;
    int temp = game->currentPiece.type;
    if (game->heldPieceType == -1) {
        // Premier hold
        game->heldPieceType = temp;
        spawnPiece(game);
    } else {
        // Échange
        int held = game->heldPieceType;
        game->heldPieceType = temp;
        game->currentPiece.type = held;
        game->currentPiece.x = BOARD_WIDTH / 2 - 2;
        game->currentPiece.y = 0;
        game->currentPiece.rotation = 0;
        game->fallTimer = 0;
        game->lockTimer = 0;
        game->lowestY = -100;
        game->dasDirection = 0; 
        game->lockDelayResets = 0;
        
        game->lastActionWasRotate = 0;
        game->tSpinBonus = 0;
    }
    game->canHold = 0; // Interdit de re-changer immédiatement
}

// [GAME LOOP LOGIC] Appelé à chaque frame
void updateGame(GameContext* game, Uint32 deltaTime) {
    if (game->state == STATE_PLAYING && !game->isPaused) {
        
        // --- GESTION DAS / ARR (Mouvement Fluide) ---
        if (game->dasDirection != 0) {
            game->dasTimer += deltaTime;
            if (game->dasPhase == 0) {
                // Phase 1 : Attente initiale (DAS Delay)
                if (game->dasTimer >= DAS_DELAY) {
                    game->dasPhase = 1;
                    game->dasTimer = 0;
                    movePiece(game, game->dasDirection, 0);
                }
            } else {
                // Phase 2 : Répétition rapide (ARR)
                if (game->dasTimer >= ARR_RATE) {
                    game->dasTimer -= ARR_RATE;
                    movePiece(game, game->dasDirection, 0);
                }
            }
        }

        // --- GRAVITÉ ET LOCK DELAY ---
        if (checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.type, game->currentPiece.rotation)) {
            // Au sol : on lance le chrono de verrouillage
            game->lockTimer += deltaTime;
            if (game->lockTimer >= LOCK_DELAY) {
                lockPiece(game);
            }
        } else {
            // En l'air : on applique la gravité
            game->lockTimer = 0;
            game->fallTimer += deltaTime;
            if (game->fallTimer >= game->fallInterval) {
                movePiece(game, 0, 1);
                game->fallTimer = 0; // [Correction] Reset timer après chute
            }
        }
    } else if (game->state == STATE_ANIMATING) {
        game->animTimer += deltaTime;
        if (game->animTimer >= 500) { // Durée animation
            performLineClear(game);
        }
    }
}

void rebindKey(GameContext* game, SDL_Keycode key) {
    bool isSecondary = (game->keyConfigColumn == 1);
    switch (game->keyConfigSelection) {
        case 0: UpdateKeyBinding(ACTION_LEFT, key, isSecondary); break;
        case 1: UpdateKeyBinding(ACTION_RIGHT, key, isSecondary); break;
        case 2: UpdateKeyBinding(ACTION_DOWN, key, isSecondary); break;
        case 3: UpdateKeyBinding(ACTION_UP, key, isSecondary); break;
        case 4: UpdateKeyBinding(ACTION_A, key, isSecondary); break;
        case 5: UpdateKeyBinding(ACTION_E, key, isSecondary); break;
        case 6: UpdateKeyBinding(ACTION_C, key, isSecondary); break;
    }
}

void handleInput(GameContext* game, SDL_Keycode key) {
    // ... (Gestion des menus, Volume, etc. identique à l'original) ...
    // Je laisse la structure pour ne pas surcharger, la logique clé est dans updateGame
    if (IsActionPressed(ACTION_MUTE, key)) { }

    if (key == SDLK_m) {
        if (audioSystem.isMuted) { audioSystem.volume = audioSystem.previousVolume; audioSystem.isMuted = false; } 
        else { audioSystem.previousVolume = audioSystem.volume; audioSystem.volume = 0.0f; audioSystem.isMuted = true; }
        ApplyVolume();
    } else if (key == SDLK_r) {
        if (audioSystem.isMuted) audioSystem.isMuted = false;
        audioSystem.volume += 0.1f; if (audioSystem.volume > 1.0f) audioSystem.volume = 1.0f;
        ApplyVolume();
    } else if (key == SDLK_f) {
        if (audioSystem.isMuted) audioSystem.isMuted = false;
        audioSystem.volume -= 0.1f; if (audioSystem.volume < 0.0f) audioSystem.volume = 0.0f;
        ApplyVolume();
    }

    if (game->state == STATE_MENU) {
        int offset = game->gameInProgress ? 1 : 0;
        int maxOptions = 3 + offset;
        switch (key) {
            case SDLK_UP: case SDLK_z: game->menuSelectedOption = (game->menuSelectedOption - 1 + maxOptions) % maxOptions; break;
            case SDLK_DOWN: case SDLK_s: game->menuSelectedOption = (game->menuSelectedOption + 1) % maxOptions; break;
            case SDLK_RETURN:
                if (game->gameInProgress && game->menuSelectedOption == 0) { game->state = STATE_PLAYING; game->isPaused = 0; } 
                else if (game->menuSelectedOption == 0 + offset) { resetGameLogic(game); } 
                else if (game->menuSelectedOption == 1 + offset) { game->state = STATE_SETTINGS; game->menuSelectedOption = 0; } 
                else if (game->menuSelectedOption == 2 + offset) { game->state = STATE_QUIT; }
                break;
            case SDLK_ESCAPE: game->state = STATE_QUIT; break;
        }
    } else if (game->state == STATE_SETTINGS) {
        int maxOptions = 9;
        switch (key) {
            case SDLK_UP: case SDLK_z: game->menuSelectedOption = (game->menuSelectedOption - 1 + maxOptions) % maxOptions; break;
            case SDLK_DOWN: case SDLK_s: game->menuSelectedOption = (game->menuSelectedOption + 1) % maxOptions; break;
            case SDLK_LEFT: case SDLK_q:
                if (game->menuSelectedOption == 0) { game->menuTextureStyle = (game->menuTextureStyle - 1 + 3) % 3; game->reloadAssetsPending = 1; }
                if (game->menuSelectedOption == 1) game->menuAutoSpeed = !game->menuAutoSpeed;
                if (game->menuSelectedOption == 2) game->menuStartLevel = (game->menuStartLevel - 1 < 1) ? 10 : game->menuStartLevel - 1;
                if (game->menuSelectedOption == 3) { audioSystem.volume -= 0.1f; if (audioSystem.volume < 0.0f) audioSystem.volume = 0.0f; ApplyVolume(); }
                if (game->menuSelectedOption == 4) { game->menuMusicTrack = (game->menuMusicTrack - 1 + 12) % 12; game->changeMusicPending = 1; }
                if (game->menuSelectedOption == 6) { game->menuResolution = (game->menuResolution - 1 + 3) % 3; game->changeResolutionPending = 1; }
                break;
            case SDLK_RIGHT: case SDLK_d:
                if (game->menuSelectedOption == 0) { game->menuTextureStyle = (game->menuTextureStyle + 1) % 3; game->reloadAssetsPending = 1; }
                if (game->menuSelectedOption == 1) game->menuAutoSpeed = !game->menuAutoSpeed;
                if (game->menuSelectedOption == 2) game->menuStartLevel = (game->menuStartLevel + 1 > 10) ? 1 : game->menuStartLevel + 1;
                if (game->menuSelectedOption == 3) { audioSystem.volume += 0.1f; if (audioSystem.volume > 1.0f) audioSystem.volume = 1.0f; audioSystem.isMuted = false; ApplyVolume(); }
                if (game->menuSelectedOption == 4) { game->menuMusicTrack = (game->menuMusicTrack + 1) % 12; game->changeMusicPending = 1; }
                if (game->menuSelectedOption == 6) { game->menuResolution = (game->menuResolution + 1) % 3; game->changeResolutionPending = 1; }
                break;
            case SDLK_RETURN:
                if (game->menuSelectedOption == 1) game->menuAutoSpeed = !game->menuAutoSpeed;
                if (game->menuSelectedOption == 5) { game->state = STATE_KEY_CONFIG; game->keyConfigSelection = 0; game->keyConfigColumn = 0; }
                if (game->menuSelectedOption == 6) { game->menuResolution = (game->menuResolution + 1) % 3; game->changeResolutionPending = 1; }
                if (game->menuSelectedOption == 7) { resetSettings(game); saveGameData(game, 0); }
                if (game->menuSelectedOption == 8) { game->state = STATE_MENU; game->menuSelectedOption = 0; saveGameData(game, 0); }
                break;
            case SDLK_ESCAPE: game->state = STATE_MENU; game->menuSelectedOption = 0; saveGameData(game, 0); break;
        }
    } else if (game->state == STATE_KEY_CONFIG) {
        if (game->isRebinding) { rebindKey(game, key); game->isRebinding = 0; saveGameData(game, 0); } 
        else {
            int maxKeys = 7;
            switch (key) {
                case SDLK_UP: game->keyConfigSelection = (game->keyConfigSelection - 1 + maxKeys) % maxKeys; break;
                case SDLK_DOWN: game->keyConfigSelection = (game->keyConfigSelection + 1) % maxKeys; break;
                case SDLK_LEFT: game->keyConfigColumn = 0; break;
                case SDLK_RIGHT: game->keyConfigColumn = 1; break;
                case SDLK_RETURN: game->isRebinding = 1; break;
                case SDLK_ESCAPE: game->state = STATE_SETTINGS; break;
            }
        }
    } else if (game->state == STATE_PLAYING) {
        if (key == SDLK_ESCAPE) {
            game->state = STATE_MENU;
            game->isPaused = 1;
            game->menuSelectedOption = 0;
            saveGameData(game, 1);
        } else if (key == SDLK_SPACE) {
            game->isPaused = !game->isPaused;
        } else if (!game->isPaused) {
            // Utilisations des Actions Abstraites
            if (IsActionPressed(ACTION_LEFT, key)) {
                movePiece(game, -1, 0);
                game->dasDirection = -1; // Active le DAS gauche
                game->dasTimer = 0;
                game->dasPhase = 0;
            }
            else if (IsActionPressed(ACTION_RIGHT, key)) {
                movePiece(game, 1, 0);
                game->dasDirection = 1; // Active le DAS droite
                game->dasTimer = 0;
                game->dasPhase = 0;
            }
            else if (IsActionPressed(ACTION_DOWN, key)) { movePiece(game, 0, 1); game->score++; }
            else if (IsActionPressed(ACTION_UP, key)) dropPiece(game);
            
            else if (IsActionPressed(ACTION_A, key)) rotatePiece(game, -1);
            else if (IsActionPressed(ACTION_E, key)) rotatePiece(game, 1);
            
            else if (IsActionPressed(ACTION_C, key)) holdPiece(game);
        }
    } else if (game->state == STATE_GAMEOVER) {
        if (key == SDLK_RETURN || key == SDLK_SPACE || key == SDLK_ESCAPE) {
            game->state = STATE_MENU;
            game->gameInProgress = 0;
        }
    }
}

// [DAS] Fonction importante pour arrêter le mouvement rapide quand on lâche la touche
void handleInputUp(GameContext* game, SDL_Keycode key) {
    if (game->state == STATE_PLAYING && !game->isPaused) {
        if (game->dasDirection == -1 && IsActionPressed(ACTION_LEFT, key)) {
            game->dasDirection = 0;
        }
        else if (game->dasDirection == 1 && IsActionPressed(ACTION_RIGHT, key)) {
            game->dasDirection = 0;
        }
    }
}