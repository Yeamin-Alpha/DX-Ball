#ifndef GAME_H
#define GAME_H

#include "constants.h"

void initStars();
void setupLevel(int lvl);
void resetBall(bool waitForLaunch);
void initGame();
void applyPerk(PerkType t);
void spawnParticles(float x,float y,Col3 col,int n = 10);
void updateParticles(float dt);
void update(float dt);
void fireBullets();

#endif

