#include "globals.h"

GameState   gState  = MENU;
int         menuSel = 0;

float       padX    = W / 2.0f;
float       padW    = PAD_DEF_W;
Ball        ball;

std::vector<Block>    blocks;
std::vector<Perk>     perks;
std::vector<Bullet>   bullets;
std::vector<Particle> particles;
std::vector<Star>     stars;
std::vector<HSEntry>  highScores;

int   lives       = INITIAL_LIVES;
int   score       = 0;
int   level       = 1;
int   totalBreak  = 0;
int   broken      = 0;
float elapsed     = 0.0f;
float speedMult   = 1.0f;

float fireTimer   = 0.0f;
float shootTimer  = 0.0f;
float shootCD     = 0.0f;


bool  leftKey     = false;
bool  rightKey    = false;
int   lastMs      = 0;
