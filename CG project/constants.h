
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

// Screen
const int   W       = 800;
const int   H       = 620;
const int   HUD_H   = 35;
const int   PLAY_H  = H - HUD_H;          // 585

// Block grid
const int   COLS    = 11;
const int   ROWS    = 7;
const float BW      = 58.0f;              // block width
const float BH      = 22.0f;              // block height
const float BG      = 4.0f;               // gap between blocks
const float BSTART_X = (W - (COLS*(BW+BG) - BG)) / 2.0f;
const float BSTART_Y = (float)PLAY_H - 60.0f;

// Paddle
const float PAD_Y        = 20.0f;
const float PAD_H        = 14.0f;
const float PAD_DEF_W    = 100.0f;
const float PAD_SPEED    = 420.0f;

// Ball
const float BALL_R       = 8.0f;
const float BALL_SPEED   = 220.0f;        // base

// Enums
enum GameState { MENU, PLAYING, PAUSED, GAMEOVER, WIN, HELP_SCR, HIGHSCORE_SCR };

enum BlockType  { BLK_NORMAL=0, BLK_HARD, BLK_BRICK, BLK_STEEL };

enum PerkType   {
    PK_LIFE=0, PK_WIDE, PK_FAST, PK_FIRE, PK_SHOOT, PK_SLOW, PK_SHRINK, PK_DEATH,
    PK_COUNT
};

// Structures
struct Col3 { float r,g,b; };

struct Block {
    float   x, y;
    bool    alive;
    BlockType type;
    int     hp;
    Col3    col;
};

struct Ball {
    float x, y, vx, vy;
    bool  active;
    bool  fire;
};

struct Perk {
    float x, y;
    PerkType type;
    bool  alive;
};

struct Bullet {
    float x, y;
    bool  alive;
};

struct Particle {
    float x, y, vx, vy;
    float life, maxLife;
    Col3  col;
};

struct Star { float x, y, bright; };

struct HSEntry { int score, level; float time; };

#endif
