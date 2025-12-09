#include "logic.h"
#include "audio.h"
#include "input.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

const int TETROMINO_SHAPES[7][4][4][2] = {
    {{{0,1}, {1,1}, {2,1}, {3,1}}, {{1,0}, {1,1}, {1,2}, {1,3}}, {{0,1}, {1,1}, {2,1}, {3,1}}, {{1,0}, {1,1}, {1,2}, {1,3}}},
    {{{1,0}, {2,0}, {1,1}, {2,1}}, {{1,0}, {2,0}, {1,1}, {2,1}}, {{1,0}, {2,0}, {1,1}, {2,1}}, {{1,0}, {2,0}, {1,1}, {2,1}}},
    {{{0,1}, {1,1}, {2,1}, {1,0}}, {{1,0}, {1,1}, {1,2}, {2,1}}, {{0,1}, {1,1}, {2,1}, {1,2}}, {{1,0}, {1,1}, {1,2}, {0,1}}},
    {{{0,0}, {1,0}, {1,1}, {2,1}}, {{1,0}, {1,1}, {0,1}, {0,2}}, {{0,0}, {1,0}, {1,1}, {2,1}}, {{1,0}, {1,1}, {0,1}, {0,2}}},
    {{{0,1}, {1,1}, {1,0}, {2,0}}, {{0,0}, {0,1}, {1,1}, {1,2}}, {{0,1}, {1,1}, {1,0}, {2,0}}, {{0,0}, {0,1}, {1,1}, {1,2}}},
    {{{0,0}, {0,1}, {1,1}, {2,1}}, {{1,0}, {2,0}, {1,1}, {1,2}}, {{0,1}, {1,1}, {2,1}, {2,2}}, {{1,0}, {1,1}, {1,2}, {0,2}}},
    {{{2,0}, {0,1}, {1,1}, {2,1}}, {{1,0}, {1,1}, {1,2}, {2,2}}, {{0,1}, {1,1}, {2,1}, {0,2}}, {{0,0}, {1,0}, {1,1}, {1,2}}}
};

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

void saveGameData(GameContext* game, int saveFullState) {
    FILE* f = fopen("save.dat", "wb");
    if (f) {
        fwrite(&game->highScore, sizeof(int), 1, f);
        fwrite(&game->bestLines, sizeof(int), 1, f);
        
        fwrite(&game->menuMusicTrack, sizeof(int), 1, f); 
        fwrite(&game->menuResolution, sizeof(int), 1, f);
        fwrite(&game->menuTextureStyle, sizeof(int), 1, f);
        fwrite(&game->menuAutoSpeed, sizeof(int), 1, f);
        fwrite(&game->menuStartLevel, sizeof(int), 1, f);

        SaveInputProfile(f); 

        int hasSave = saveFullState;
        fwrite(&hasSave, sizeof(int), 1, f);
        
        if (saveFullState) {
            fwrite(game, sizeof(GameContext), 1, f);
        }
        fclose(f);
    }
}

void loadGameData(GameContext* game) {
    FILE* f = fopen("save.dat", "rb");
    if (f) {
        int tempScore, tempLines, tempMusic, tempRes, tempTex, tempSpeed, tempLevel, hasSave;
        
        if (fread(&tempScore, sizeof(int), 1, f) == 1) game->highScore = tempScore;
        if (fread(&tempLines, sizeof(int), 1, f) == 1) game->bestLines = tempLines;
        
        if (fread(&tempMusic, sizeof(int), 1, f) == 1) game->menuMusicTrack = tempMusic;
        if (fread(&tempRes, sizeof(int), 1, f) == 1) game->menuResolution = tempRes;
        
        if (fread(&tempTex, sizeof(int), 1, f) == 1) { 
            game->menuTextureStyle = tempTex;
            game->reloadAssetsPending = 1; 
        }
        if (fread(&tempSpeed, sizeof(int), 1, f) == 1) game->menuAutoSpeed = tempSpeed;
        if (fread(&tempLevel, sizeof(int), 1, f) == 1) game->menuStartLevel = tempLevel;
        
        LoadInputProfile(f);

        if (fread(&hasSave, sizeof(int), 1, f) == 1 && hasSave == 1) {
            fread(game, sizeof(GameContext), 1, f);
            game->state = STATE_MENU; 
            game->isPaused = 1;      
        } else {
            game->gameInProgress = 0;
        }
        fclose(f);
    } else {
        game->highScore = 0; 
        game->bestLines = 0; 
        game->gameInProgress = 0;
    }
}

void checkHighScore(GameContext* game) {
    int changed = 0;
    if (game->score > game->highScore) { game->highScore = game->score; changed = 1; }
    if (game->linesCleared > game->bestLines) { game->bestLines = game->linesCleared; changed = 1; }
}

void initDefaultKeys(GameContext* game) {}

void initGame(GameContext* game) {
    srand(time(NULL));
    
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

    game->nextPieceType = getRandomPieceType(game); 
    game->heldPieceType = -1; 

    initDefaultKeys(game); 
    loadGameData(game);    
    
    game->masterVolume = 0;
    game->isMuted = 1;

    game->changeResolutionPending = 1; 
    game->reloadAssetsPending = 1; 
    game->changeMusicPending = 1;  
}

// Fonction de réinitialisation des paramètres
void resetSettings(GameContext* game) {
    game->menuTextureStyle = 0;
    game->menuAutoSpeed = 1;
    game->menuStartLevel = 1;
    game->menuMusicTrack = 0;
    game->menuResolution = 0;
    
    // Applique immédiatement
    game->reloadAssetsPending = 1;
    game->changeMusicPending = 1;
    game->changeResolutionPending = 1;
    
    // Réinit touches par défaut
    InitInputProfile();
}

static int checkCollision(GameContext* game, int px, int py, int type, int rot) {
    for (int i = 0; i < 4; i++) {
        int bx = px + TETROMINO_SHAPES[type][rot][i][1];
        int by = py + TETROMINO_SHAPES[type][rot][i][0];
        
        if (bx < 0 || bx >= BOARD_WIDTH || by >= BOARD_HEIGHT) return 1;
        if (by >= 0 && game->board[by][bx] != -1) return 1;
    }
    return 0;
}

static void spawnPiece(GameContext* game) {
    game->currentPiece.type = game->nextPieceType;
    game->currentPiece.rotation = 0;
    game->currentPiece.x = BOARD_WIDTH / 2 - 2;
    game->currentPiece.y = 0;
    game->nextPieceType = getRandomPieceType(game); 
    game->canHold = 1;
    
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

    double multiplier = pow(0.75, game->level - 1);
    game->fallInterval = (int)(multiplier * 1000);
    if (game->fallInterval < 50) game->fallInterval = 50; 

    game->nextPieceType = getRandomPieceType(game);
    spawnPiece(game);
    game->fallTimer = 0;
    game->state = STATE_PLAYING;
}

static void performLineClear(GameContext* game) {
    int linesFound = game->linesToClearCount;
    for (int i = 0; i < linesFound; i++) {
        int y = game->linesToClear[i];
        for (int row = y; row > 0; row--) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                game->board[row][x] = game->board[row - 1][x];
            }
        }
        for (int x = 0; x < BOARD_WIDTH; x++) game->board[0][x] = -1;
    }
    
    if (linesFound > 0) {
        game->linesCleared += linesFound;
        int points[] = {0, 40, 100, 300, 1200};
        game->score += points[linesFound] * game->level;
        
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
            if (game->board[y][x] == -1) { full = 0; break; }
        }
        if (full) {
            game->linesToClear[game->linesToClearCount++] = y;
        }
    }
    
    if (game->linesToClearCount > 0) {
        game->state = STATE_ANIMATING; 
        game->animTimer = 0;
        game->playSoundClearPending = 1; 
    } else {
        spawnPiece(game);
    }
}

static void lockPiece(GameContext* game) {
    for (int i = 0; i < 4; i++) {
        int bx = game->currentPiece.x + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][1];
        int by = game->currentPiece.y + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][0];
        
        if (by >= 0 && by < BOARD_HEIGHT && bx >= 0 && bx < BOARD_WIDTH) {
            game->board[by][bx] = game->currentPiece.type;
        } else if (by < 0) {
            game->state = STATE_GAMEOVER;
            game->gameInProgress = 0;
            checkHighScore(game);
            saveGameData(game, 0); 
            return;
        }
    }
    detectLines(game);
}

static void movePiece(GameContext* game, int dx, int dy) {
    if (game->state != STATE_PLAYING || game->isPaused) return;
    
    if (!checkCollision(game, game->currentPiece.x + dx, game->currentPiece.y + dy, game->currentPiece.type, game->currentPiece.rotation)) {
        game->currentPiece.x += dx;
        game->currentPiece.y += dy;
        if (dy > 0) game->fallTimer = 0;
    } else if (dy > 0) {
        lockPiece(game);
    }
}

static void rotatePiece(GameContext* game, int dir) {
    if (game->state != STATE_PLAYING || game->isPaused) return;
    int newRot = (game->currentPiece.rotation + dir + 4) % 4;
    
    int kicks[6][2] = {{0,0}, {-1,0}, {1,0}, {0,-1}, {-2,0}, {2,0}};
    for (int i = 0; i < 6; i++) {
        if (game->currentPiece.type != 0 && abs(kicks[i][0]) == 2) continue;
        
        if (!checkCollision(game, game->currentPiece.x + kicks[i][0], game->currentPiece.y + kicks[i][1], game->currentPiece.type, newRot)) {
            game->currentPiece.x += kicks[i][0];
            game->currentPiece.y += kicks[i][1];
            game->currentPiece.rotation = newRot;
            return;
        }
    }
}

static void dropPiece(GameContext* game) {
    if (game->state != STATE_PLAYING || game->isPaused) return;
    while (!checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.type, game->currentPiece.rotation)) {
        game->currentPiece.y++;
        game->score += 2;
    }
    lockPiece(game);
}

static void holdPiece(GameContext* game) {
    if (game->state != STATE_PLAYING || game->isPaused || !game->canHold) return;
    
    int temp = game->currentPiece.type;
    if (game->heldPieceType == -1) { 
        game->heldPieceType = temp;
        spawnPiece(game);
    } else {
        int held = game->heldPieceType;
        game->heldPieceType = temp;
        game->currentPiece.type = held;
        game->currentPiece.x = BOARD_WIDTH / 2 - 2;
        game->currentPiece.y = 0;
        game->currentPiece.rotation = 0;
    }
    game->canHold = 0;
}

void updateGame(GameContext* game, Uint32 deltaTime) {
    if (game->state == STATE_PLAYING && !game->isPaused) {
        game->fallTimer += deltaTime;
        if (game->fallTimer >= game->fallInterval) { 
            movePiece(game, 0, 1);
            game->fallTimer = 0;
        }
    } else if (game->state == STATE_ANIMATING) {
        game->animTimer += deltaTime;
        if (game->animTimer >= 500) {
            performLineClear(game);
        }
    }
}

void rebindKey(GameContext* game, SDL_Keycode key) {
    bool isSecondary = (game->keyConfigColumn == 1);
    switch(game->keyConfigSelection) {
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
    if (IsActionPressed(ACTION_MUTE, key)) { }
    
    if (key == SDLK_m) {
        if (audioSystem.isMuted) { audioSystem.volume = audioSystem.previousVolume; audioSystem.isMuted = false; }
        else { audioSystem.previousVolume = audioSystem.volume; audioSystem.volume = 0.0f; audioSystem.isMuted = true; }
        ApplyVolume();
    }
    else if (key == SDLK_r) { if (audioSystem.isMuted) audioSystem.isMuted = false; audioSystem.volume += 0.1f; if (audioSystem.volume > 1.0f) audioSystem.volume = 1.0f; ApplyVolume(); }
    else if (key == SDLK_f) { if (audioSystem.isMuted) audioSystem.isMuted = false; audioSystem.volume -= 0.1f; if (audioSystem.volume < 0.0f) audioSystem.volume = 0.0f; ApplyVolume(); }

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
    } 
    else if (game->state == STATE_SETTINGS) {
        int maxOptions = 9; // Augmenté à 9 pour inclure "Réinitialiser"
        switch(key) {
            case SDLK_UP: case SDLK_z: game->menuSelectedOption = (game->menuSelectedOption - 1 + maxOptions) % maxOptions; break;
            case SDLK_DOWN: case SDLK_s: game->menuSelectedOption = (game->menuSelectedOption + 1) % maxOptions; break;
            
            case SDLK_LEFT: case SDLK_q:
                if (game->menuSelectedOption == 0) { game->menuTextureStyle = !game->menuTextureStyle; game->reloadAssetsPending = 1; }
                if (game->menuSelectedOption == 1) game->menuAutoSpeed = !game->menuAutoSpeed;
                if (game->menuSelectedOption == 2) game->menuStartLevel = (game->menuStartLevel - 1 < 1) ? 10 : game->menuStartLevel - 1;
                if (game->menuSelectedOption == 3) { audioSystem.volume -= 0.1f; if (audioSystem.volume < 0.0f) audioSystem.volume = 0.0f; ApplyVolume(); }
                // Modulo 11 car 10 pistes + "Aucune"
                if (game->menuSelectedOption == 4) { game->menuMusicTrack = (game->menuMusicTrack - 1 + 11) % 11; game->changeMusicPending = 1; }
                if (game->menuSelectedOption == 6) { game->menuResolution = (game->menuResolution - 1 + 3) % 3; game->changeResolutionPending = 1; }
                break;
            
            case SDLK_RIGHT: case SDLK_d:
                if (game->menuSelectedOption == 0) { game->menuTextureStyle = !game->menuTextureStyle; game->reloadAssetsPending = 1; }
                if (game->menuSelectedOption == 1) game->menuAutoSpeed = !game->menuAutoSpeed;
                if (game->menuSelectedOption == 2) game->menuStartLevel = (game->menuStartLevel + 1 > 10) ? 1 : game->menuStartLevel + 1;
                if (game->menuSelectedOption == 3) { audioSystem.volume += 0.1f; if (audioSystem.volume > 1.0f) audioSystem.volume = 1.0f; audioSystem.isMuted = false; ApplyVolume(); }
                // Modulo 11
                if (game->menuSelectedOption == 4) { game->menuMusicTrack = (game->menuMusicTrack + 1) % 11; game->changeMusicPending = 1; }
                if (game->menuSelectedOption == 6) { game->menuResolution = (game->menuResolution + 1) % 3; game->changeResolutionPending = 1; }
                break;
            
            case SDLK_RETURN:
                if (game->menuSelectedOption == 1) game->menuAutoSpeed = !game->menuAutoSpeed;
                if (game->menuSelectedOption == 5) { game->state = STATE_KEY_CONFIG; game->keyConfigSelection = 0; game->keyConfigColumn = 0; }
                if (game->menuSelectedOption == 6) { game->menuResolution = (game->menuResolution + 1) % 3; game->changeResolutionPending = 1; }
                // Bouton Réinitialiser (Index 7)
                if (game->menuSelectedOption == 7) { resetSettings(game); saveGameData(game, 0); }
                // Bouton Retour (Index 8)
                if (game->menuSelectedOption == 8) { game->state = STATE_MENU; game->menuSelectedOption = 0; saveGameData(game, 0); }
                break;
            case SDLK_ESCAPE: game->state = STATE_MENU; game->menuSelectedOption = 0; saveGameData(game, 0); break;
        }
    }
    else if (game->state == STATE_KEY_CONFIG) {
        if (game->isRebinding) {
            rebindKey(game, key);
            game->isRebinding = 0;
            saveGameData(game, 0);
        } else {
            int maxKeys = 7;
            switch(key) {
                case SDLK_UP: game->keyConfigSelection = (game->keyConfigSelection - 1 + maxKeys) % maxKeys; break;
                case SDLK_DOWN: game->keyConfigSelection = (game->keyConfigSelection + 1) % maxKeys; break;
                case SDLK_LEFT: game->keyConfigColumn = 0; break;
                case SDLK_RIGHT: game->keyConfigColumn = 1; break;
                case SDLK_RETURN: game->isRebinding = 1; break;
                case SDLK_ESCAPE: game->state = STATE_SETTINGS; break;
            }
        }
    }
    else if (game->state == STATE_PLAYING) {
        if (key == SDLK_ESCAPE) { game->state = STATE_MENU; game->isPaused = 1; game->menuSelectedOption = 0; saveGameData(game, 1); }
        else if (key == SDLK_SPACE) { game->isPaused = !game->isPaused; }
        else if (!game->isPaused) {
            if (IsActionPressed(ACTION_LEFT, key)) movePiece(game, -1, 0);
            else if (IsActionPressed(ACTION_RIGHT, key)) movePiece(game, 1, 0);
            else if (IsActionPressed(ACTION_DOWN, key)) { movePiece(game, 0, 1); game->score++; }
            else if (IsActionPressed(ACTION_UP, key)) dropPiece(game);
            else if (IsActionPressed(ACTION_A, key)) rotatePiece(game, 1);
            else if (IsActionPressed(ACTION_E, key)) rotatePiece(game, -1);
            else if (IsActionPressed(ACTION_C, key)) holdPiece(game);
        }
    } 
    else if (game->state == STATE_GAMEOVER) {
        if (key == SDLK_RETURN || key == SDLK_SPACE || key == SDLK_ESCAPE) { game->state = STATE_MENU; game->gameInProgress = 0; }
    }
}