#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "constants.h"

// Game state
extern GameState   gState;
extern int         menuSel;

// Gameplay objects
extern float       padX;
extern float       padW;
extern Ball        ball;

extern std::vector<Block>    blocks;
extern std::vector<Perk>     perks;
extern std::vector<Bullet>   bullets;
extern std::vector<Particle> particles;
extern std::vector<Star>     stars;
extern std::vector<HSEntry>  highScores;

// Numbers
extern int   lives;
extern int   score;
extern int   level;
extern int   totalBreak;
extern int   broken;
extern float elapsed;
extern float speedMult;

// Perk timers
extern float fireTimer;
extern float shootTimer;
extern float shootCD;

// Input flags
extern bool  leftKey;
extern bool  rightKey;
extern int   lastMs;

#endif
