#ifndef RENDER_H
#define RENDER_H
#include "constants.h"
void drawStars();
void drawParticles();
void drawBrickBlock(float x,float y,Col3 c);
void drawSteelBlock(float x,float y);
void drawBlock(const Block& b);
void drawPaddle();
void drawBallObj();
void drawPerks();
void drawBullets();
void drawHUD();
void drawMenu();
void drawPause();
void drawGameOver();
void drawWin();
void drawHelpScreen();
void drawHighScoreScreen();
void display();

// Helper drawing primitives
void fillRect(float x,float y,float w,float h);
void lineRect(float x,float y,float w,float h,float lw=1.5f);
void circ(float cx,float cy,float r,int n=28,bool fill=true);
void drawStr(float x,float y,const char* s, void* font);

#endif
