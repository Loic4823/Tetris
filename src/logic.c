#include "logic.h"
#include "input.h" 
#include "audio.h" 
#include <stdlib.h>
#include <time.h>
#include <stdio.h> 

// --- DEFINITION DES FORMES (SRS STANDARD) ---
const int TETROMINO_SHAPES[7][4][4][2] = {
    // I (Barre) - Cyan
    {
        {{1, 0}, {1, 1}, {1, 2}, {1, 3}}, 
        {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, 
        {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, 
        {{0, 1}, {1, 1}, {2, 1}, {3, 1}}
    },
    // J (Bleu)
    {
        {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
        {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
        {{0, 1}, {1, 1}, {2, 0}, {2, 1}}
    },
    // L (Orange)
    {
        {{1, 0}, {1, 1}, {1, 2}, {0, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
        {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
        {{0, 0}, {0, 1}, {1, 1}, {2, 1}}
    },
    // O (Carré - Jaune)
    {
        {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
        {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
        {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
        {{0, 1}, {0, 2}, {1, 1}, {1, 2}}
    },
    // S (Vert)
    {
        {{1, 0}, {1, 1}, {0, 1}, {0, 2}},
        {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
        {{1, 0}, {1, 1}, {0, 1}, {0, 2}},
        {{0, 1}, {1, 1}, {1, 2}, {2, 2}}
    },
    // T (Violet)
    {
        {{1, 0}, {1, 1}, {0, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {1, 2}},
        {{1, 0}, {1, 1}, {2, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {1, 0}}
    },
    // Z (Rouge)
    {
        {{0, 0}, {0, 1}, {1, 1}, {1, 2}},
        {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
        {{0, 0}, {0, 1}, {1, 1}, {1, 2}},
        {{0, 2}, {1, 1}, {1, 2}, {2, 1}}
    }
};

// --- TABLES DE WALL KICKS SRS (Super Rotation System) ---
// Format : [Transition Index][Test 0..4][x, y]
// Transitions : 0->1, 1->0, 1->2, 2->1, 2->3, 3->2, 3->0, 0->3

// Table pour J, L, S, T, Z
const int KICKS_JLSTZ[8][5][2] = {
    {{0, 0}, {-1, 0}, {-1, -1}, { 0,  2}, {-1,  2}}, // 0->1
    {{0, 0}, { 1, 0}, { 1,  1}, { 0, -2}, { 1, -2}}, // 1->0
    {{0, 0}, { 1, 0}, { 1, -1}, { 0,  2}, { 1,  2}}, // 1->2
    {{0, 0}, {-1, 0}, {-1,  1}, { 0, -2}, {-1, -2}}, // 2->1
    {{0, 0}, { 1, 0}, { 1,  1}, { 0, -2}, { 1, -2}}, // 2->3
    {{0, 0}, {-1, 0}, {-1, -1}, { 0,  2}, {-1,  2}}, // 3->2
    {{0, 0}, {-1, 0}, {-1, -1}, { 0,  2}, {-1,  2}}, // 3->0
    {{0, 0}, { 1, 0}, { 1,  1}, { 0, -2}, { 1, -2}}  // 0->3
};

// Table pour I (Barre)
const int KICKS_I[8][5][2] = {
    {{0, 0}, {-2, 0}, { 1, 0}, {-2, -1}, { 1,  2}}, // 0->1
    {{0, 0}, { 2, 0}, {-1, 0}, { 2,  1}, {-1, -2}}, // 1->0
    {{0, 0}, {-1, 0}, { 2, 0}, {-1,  2}, { 2, -1}}, // 1->2
    {{0, 0}, { 1, 0}, {-2, 0}, { 1, -2}, {-2,  1}}, // 2->1
    {{0, 0}, { 2, 0}, {-1, 0}, { 2,  1}, {-1, -2}}, // 2->3
    {{0, 0}, {-2, 0}, { 1, 0}, {-2, -1}, { 1,  2}}, // 3->2
    {{0, 0}, { 1, 0}, {-2, 0}, { 1, -2}, {-2,  1}}, // 3->0
    {{0, 0}, {-1, 0}, { 2, 0}, {-1,  2}, { 2, -1}}  // 0->3
};

int checkCollision(GameContext* game, int x, int y, int rotation) {
    for (int i = 0; i < 4; i++) {
        int bx = x + TETROMINO_SHAPES[game->currentPiece.type][rotation][i][1];
        int by = y + TETROMINO_SHAPES[game->currentPiece.type][rotation][i][0];
        
        if (bx < 0 || bx >= BOARD_WIDTH || by >= BOARD_HEIGHT) return 1;
        if (by >= 0 && game->board[by][bx] != -1) return 1;
    }
    return 0;
}

void movePiece(GameContext* game, int dx, int dy) {
    if (!checkCollision(game, game->currentPiece.x + dx, game->currentPiece.y + dy, game->currentPiece.rotation)) {
        game->currentPiece.x += dx;
        game->currentPiece.y += dy;
        
        if (dy == 0 && checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.rotation)) {
            if (game->lockDelayResets < MAX_LOCK_RESETS) {
                game->lastDropTime = SDL_GetTicks();
                game->lockDelayResets++;
            }
        }
    }
}

void rotatePiece(GameContext* game, int direction) {
    // 1. Calcul de la nouvelle rotation
    int oldRotation = game->currentPiece.rotation;
    int newRotation = (oldRotation + direction);
    if (newRotation < 0) newRotation = 3;
    if (newRotation > 3) newRotation = 0;
    
    // Le Carré (O - Type 3) ne tourne pas
    if (game->currentPiece.type == 3) return;

    // 2. Détermination de l'index de transition pour la table SRS
    // 0->1:0, 1->0:1, 1->2:2, 2->1:3, 2->3:4, 3->2:5, 3->0:6, 0->3:7
    int kickIndex = 0;
    if (oldRotation == 0 && newRotation == 1) kickIndex = 0;
    else if (oldRotation == 1 && newRotation == 0) kickIndex = 1;
    else if (oldRotation == 1 && newRotation == 2) kickIndex = 2;
    else if (oldRotation == 2 && newRotation == 1) kickIndex = 3;
    else if (oldRotation == 2 && newRotation == 3) kickIndex = 4;
    else if (oldRotation == 3 && newRotation == 2) kickIndex = 5;
    else if (oldRotation == 3 && newRotation == 0) kickIndex = 6;
    else if (oldRotation == 0 && newRotation == 3) kickIndex = 7;

    // 3. Sélection de la bonne table (I ou Autres)
    const int (*kicks)[5][2];
    if (game->currentPiece.type == 0) { // Type 0 = Barre (I)
        kicks = &KICKS_I[kickIndex];
    } else {
        kicks = &KICKS_JLSTZ[kickIndex];
    }

    // 4. Test des 5 positions (SRS)
    int rotated = 0;
    for (int i = 0; i < 5; i++) {
        int testX = game->currentPiece.x + (*kicks)[i][0];
        int testY = game->currentPiece.y + (*kicks)[i][1]; // Note: SDL Y est vers le bas, la table est ajustée

        if (!checkCollision(game, testX, testY, newRotation)) {
            game->currentPiece.x = testX;
            game->currentPiece.y = testY;
            game->currentPiece.rotation = newRotation;
            rotated = 1;
            break; // On a trouvé une position valide !
        }
    }

    // Gestion du Lock Delay après rotation réussie
    if (rotated) {
        if (checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.rotation)) {
            if (game->lockDelayResets < MAX_LOCK_RESETS) {
                game->lastDropTime = SDL_GetTicks();
                game->lockDelayResets++;
            }
        }
    }
}

void holdPiece(GameContext* game) {
    if (!game->canHold) return;

    if (game->heldPieceType == -1) {
        game->heldPieceType = game->currentPiece.type;
        game->currentPiece.type = game->nextPieceType;
        game->nextPieceType = getRandomPieceType();
        game->currentPiece.x = BOARD_WIDTH / 2 - 2;
        game->currentPiece.y = 0;
        game->currentPiece.rotation = 0;
    } else {
        int temp = game->currentPiece.type;
        game->currentPiece.type = game->heldPieceType;
        game->heldPieceType = temp;
        game->currentPiece.x = BOARD_WIDTH / 2 - 2;
        game->currentPiece.y = 0;
        game->currentPiece.rotation = 0;
    }
    game->canHold = 0; 
    game->lockDelayResets = 0; 
}

void dropPiece(GameContext* game) {
    while (!checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.rotation)) {
        game->currentPiece.y++;
        game->score += 2; 
    }
    game->lastDropTime = 0; 
}

void clearLines(GameContext* game) {
    int linesClearedHere = 0;
    game->linesToClearCount = 0;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        int full = 1;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x] == -1) {
                full = 0;
                break;
            }
        }
        if (full) {
            game->linesToClear[game->linesToClearCount++] = y;
            linesClearedHere++;
        }
    }

    if (linesClearedHere > 0) {
        game->state = STATE_ANIMATING;
        game->animStartTime = SDL_GetTicks();
        game->animTimer = 0;
        
        game->linesCleared += linesClearedHere;
        int points = 0;
        switch(linesClearedHere) {
            case 1: points = 100; break;
            case 2: points = 300; break;
            case 3: points = 500; break;
            case 4: points = 800; break;
        }
        game->score += points * game->level;
        if (game->score > game->highScore) game->highScore = game->score;
        if (game->linesCleared > game->bestLines) game->bestLines = game->linesCleared;
        
        if (game->linesCleared >= game->level * 10) {
            game->level++;
            if (game->menuAutoSpeed) {
                game->speed = (game->speed > 100) ? game->speed - 50 : 100;
            }
        }
    }
}

void finishLineClear(GameContext* game) {
    for (int i = 0; i < game->linesToClearCount; i++) {
        int lineY = game->linesToClear[i];
        for (int y = lineY; y > 0; y--) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                game->board[y][x] = game->board[y-1][x];
            }
        }
        for (int x = 0; x < BOARD_WIDTH; x++) {
            game->board[0][x] = -1;
        }
    }
    game->linesToClearCount = 0;
    game->state = STATE_PLAYING;
}

void lockPiece(GameContext* game) {
    for (int i = 0; i < 4; i++) {
        int bx = game->currentPiece.x + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][1];
        int by = game->currentPiece.y + TETROMINO_SHAPES[game->currentPiece.type][game->currentPiece.rotation][i][0];
        if (by >= 0) game->board[by][bx] = game->currentPiece.type;
    }
    clearLines(game);
    
    game->currentPiece.type = game->nextPieceType;
    game->nextPieceType = getRandomPieceType();
    game->currentPiece.x = BOARD_WIDTH / 2 - 2;
    game->currentPiece.y = 0;
    game->currentPiece.rotation = 0;
    game->canHold = 1;
    game->lockDelayResets = 0;

    if (checkCollision(game, game->currentPiece.x, game->currentPiece.y, game->currentPiece.rotation)) {
        game->state = STATE_GAMEOVER;
        game->gameInProgress = 0;
    }
}

int getRandomPieceType() {
    return rand() % 7;
}

void initGame(GameContext* game) {
    srand(time(NULL));
    game->highScore = 0;
    game->bestLines = 0;
    game->menuSelectedOption = 0;
    game->state = STATE_MENU;
    
    game->menuTextureStyle = 0; 
    game->menuAutoSpeed = 1;    
    game->menuStartLevel = 1;   
    game->menuMusicTrack = 0;   
    game->menuResolution = 0;   
    
    game->isMuted = 0;
    game->masterVolume = 0; 
    
    game->keyConfigSelection = 0;
    game->keyConfigColumn = 0;
    game->isRebinding = 0;
    game->gameInProgress = 0;
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
    
    if (game->menuAutoSpeed) game->speed = 1000 - (game->level - 1) * 100;
    else game->speed = 1000;
    if (game->speed < 100) game->speed = 100;

    game->nextPieceType = getRandomPieceType();
    game->currentPiece.type = getRandomPieceType();
    game->currentPiece.x = BOARD_WIDTH / 2 - 2;
    game->currentPiece.y = 0;
    game->currentPiece.rotation = 0;
    game->heldPieceType = -1;
    game->canHold = 1;
    game->lockDelayResets = 0;
    
    game->lastDropTime = SDL_GetTicks();
    game->isPaused = 0;
    game->state = STATE_PLAYING;
    game->gameInProgress = 1;
    
    game->moveDirection = 0;
    game->moveTimer = 0;
}

void handleInput(GameContext* game, SDL_Keycode key) {
    if (game->state == STATE_KEY_CONFIG) {
        HandleKeyConfigInput(game, key);
        return;
    }

    if (game->state == STATE_MENU) {
        if (key == SDLK_UP) {
            game->menuSelectedOption--;
            int maxOpt = game->gameInProgress ? 5 : 3; 
            if (game->menuSelectedOption < 0) game->menuSelectedOption = maxOpt - 1;
        } else if (key == SDLK_DOWN) {
            game->menuSelectedOption++;
            int maxOpt = game->gameInProgress ? 5 : 3;
            if (game->menuSelectedOption >= maxOpt) game->menuSelectedOption = 0;
        } else if (key == SDLK_RETURN) {
            int offset = game->gameInProgress ? 1 : 0;
            if (game->gameInProgress) {
                if (game->menuSelectedOption == 0) game->state = STATE_PLAYING; 
                else if (game->menuSelectedOption == 1) resetGameLogic(game);   
            } else {
                if (game->menuSelectedOption == 0) resetGameLogic(game);        
            }
            if (game->menuSelectedOption == 1 + offset) {
                game->state = STATE_SETTINGS;
                game->menuSelectedOption = 0;
            }
            if (game->menuSelectedOption == 2 + offset) exit(0);
        }
    } else if (game->state == STATE_SETTINGS) {
        if (key == SDLK_UP) {
            game->menuSelectedOption--;
            if (game->menuSelectedOption < 0) game->menuSelectedOption = 8;
        } else if (key == SDLK_DOWN) {
            game->menuSelectedOption++;
            if (game->menuSelectedOption > 8) game->menuSelectedOption = 0;
        } else if (key == SDLK_LEFT || key == SDLK_RIGHT) {
            int dir = (key == SDLK_RIGHT) ? 1 : -1;
            if (game->menuSelectedOption == 0) { game->menuTextureStyle += dir; if(game->menuTextureStyle > 2) game->menuTextureStyle = 0; if(game->menuTextureStyle < 0) game->menuTextureStyle = 2; }
            if (game->menuSelectedOption == 1) game->menuAutoSpeed = !game->menuAutoSpeed;
            if (game->menuSelectedOption == 2) { game->menuStartLevel += dir; if(game->menuStartLevel < 1) game->menuStartLevel = 1; if(game->menuStartLevel > 20) game->menuStartLevel = 20; }
            
            if (game->menuSelectedOption == 3) { 
                if (dir > 0) {
                    game->masterVolume += 8;
                    if (game->masterVolume > 128) game->masterVolume = 128;
                } else {
                    game->masterVolume -= 8;
                    if (game->masterVolume < 0) game->masterVolume = 0;
                }
            }

            if (game->menuSelectedOption == 4) { game->menuMusicTrack += dir; if(game->menuMusicTrack < 0) game->menuMusicTrack = 10; if(game->menuMusicTrack > 10) game->menuMusicTrack = 0; }
            if (game->menuSelectedOption == 6) { game->menuResolution += dir; if(game->menuResolution < 0) game->menuResolution = 2; if(game->menuResolution > 2) game->menuResolution = 0; }
        } else if (key == SDLK_RETURN) {
            if (game->menuSelectedOption == 5) game->state = STATE_KEY_CONFIG;
            if (game->menuSelectedOption == 7) { 
                game->menuTextureStyle = 0; game->menuAutoSpeed = 1; game->menuStartLevel = 1; 
                game->menuMusicTrack = 0; game->menuResolution = 0; 
                game->masterVolume = 0;
            }
            if (game->menuSelectedOption == 8) {
                game->state = STATE_MENU;
                game->menuSelectedOption = 0;
            }
        }
    } else if (game->state == STATE_GAMEOVER) {
        if (key == SDLK_RETURN) game->state = STATE_MENU;
    } else if (game->state == STATE_PLAYING) {
        if (key == SDLK_ESCAPE) {
            game->state = STATE_MENU;
            game->isPaused = 1;
            game->menuSelectedOption = 0;
        } else if (key == SDLK_SPACE) {
            game->isPaused = !game->isPaused;
        } else if (!game->isPaused) {
            if (IsActionPressed(ACTION_LEFT, key)) {
                movePiece(game, -1, 0); 
                game->moveDirection = -1;
                game->moveTimer = DAS_DELAY;
            } 
            else if (IsActionPressed(ACTION_RIGHT, key)) {
                movePiece(game, 1, 0);
                game->moveDirection = 1;
                game->moveTimer = DAS_DELAY;
            }
            else if (IsActionPressed(ACTION_DOWN, key)) { 
                movePiece(game, 0, 1); game->score++; 
            }
            else if (IsActionPressed(ACTION_UP, key)) dropPiece(game);
            else if (IsActionPressed(ACTION_A, key)) rotatePiece(game, -1);
            else if (IsActionPressed(ACTION_E, key)) rotatePiece(game, 1);
            else if (IsActionPressed(ACTION_C, key)) holdPiece(game);
        }
    }
}

void handleKeyUp(GameContext* game, SDL_Keycode key) {
    if (game->state == STATE_PLAYING && !game->isPaused) {
        if (IsActionPressed(ACTION_LEFT, key) && game->moveDirection == -1) {
            game->moveDirection = 0;
            game->moveTimer = 0;
        }
        else if (IsActionPressed(ACTION_RIGHT, key) && game->moveDirection == 1) {
            game->moveDirection = 0;
            game->moveTimer = 0;
        }
    }
}

void updateGame(GameContext* game, Uint32 deltaTime) {
    if (game->state == STATE_ANIMATING) {
        game->animTimer += deltaTime;
        if (game->animTimer > 500) { 
            finishLineClear(game);
        }
        return;
    }

    if (game->state != STATE_PLAYING || game->isPaused) return;

    if (game->moveDirection != 0) {
        game->moveTimer -= deltaTime;
        if (game->moveTimer <= 0) {
            movePiece(game, game->moveDirection, 0); 
            game->moveTimer = ARR_RATE;             
        }
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - game->lastDropTime > game->speed) {
        if (!checkCollision(game, game->currentPiece.x, game->currentPiece.y + 1, game->currentPiece.rotation)) {
            game->currentPiece.y++;
        } else {
            lockPiece(game);
        }
        game->lastDropTime = currentTime;
    }
}