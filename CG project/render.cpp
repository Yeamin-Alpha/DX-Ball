#include <GL/freeglut.h>
#include <cstdio>
#include <cmath>
#include "render.h"
#include "constants.h"
#include "globals.h"

// ---------- PRIMITIVES ----------
void fillRect(float x,float y,float w,float h){
    glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
}

void lineRect(float x,float y,float w,float h,float lw){
    glLineWidth(lw);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y); glVertex2f(x+w,y);
    glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd();
    glLineWidth(1.0f);
}

void circ(float cx,float cy,float r,int n,bool fill){
    glBegin(fill?GL_TRIANGLE_FAN:GL_LINE_LOOP);
    if(fill) glVertex2f(cx,cy);
    for(int i=0;i<=n;i++){
        float a=2.0f*(float)M_PI*i/n;
        glVertex2f(cx+r*cosf(a), cy+r*sinf(a));
    }
    glEnd();
}

void drawStr(float x,float y,const char* s, void* font){
    glRasterPos2f(x,y);
    for(;*s;s++) glutBitmapCharacter(font,*s);
}

// ---------- BACKGROUND ----------
void drawStars(){
    glPointSize(1.0f);
    for(auto&s:stars){
        glColor3f(s.bright,s.bright,s.bright);
        glPointSize(s.bright>0.75f?2.0f:1.0f);
        glBegin(GL_POINTS);
        glVertex2f(s.x,s.y);
        glEnd();
    }
    glPointSize(1.0f);
}

// ---------- PARTICLES (rendering) ----------
void drawParticles(){
    for(auto&p:particles){
        float a = p.life / p.maxLife;
        glColor3f(p.col.r * a, p.col.g * a, p.col.b * a);
        glPointSize(3.5f * a + 0.5f);
        glBegin(GL_POINTS);
        glVertex2f(p.x, p.y);
        glEnd();
    }
    glPointSize(1.0f);
}

// ---------- BLOCKS ----------
void drawBrickBlock(float x,float y,Col3 c){
    glColor3f(c.r,c.g,c.b);
    fillRect(x,y,BW,BH);
    // mortar grid
    glColor3f(c.r*0.45f,c.g*0.45f,c.b*0.45f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x,     y+BH/2); glVertex2f(x+BW,  y+BH/2);
    glVertex2f(x+BW*0.25f, y);      glVertex2f(x+BW*0.25f, y+BH/2);
    glVertex2f(x+BW*0.75f, y);      glVertex2f(x+BW*0.75f, y+BH/2);
    glVertex2f(x+BW*0.5f,  y+BH/2);glVertex2f(x+BW*0.5f,  y+BH);
    glEnd();
    glLineWidth(1.0f);
}

void drawSteelBlock(float x,float y){
    glColor3f(0.55f,0.55f,0.6f);
    fillRect(x,y,BW,BH);
    glColor3f(0.75f,0.75f,0.8f);
    fillRect(x+3,y+BH-4,BW-6,3);
    glColor3f(0.35f,0.35f,0.4f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x+5,y+4);   glVertex2f(x+BW-5,y+BH-4);
    glVertex2f(x+BW-5,y+4);glVertex2f(x+5,y+BH-4);
    glEnd();
    glLineWidth(1.0f);
}

void drawBlock(const Block& b){
    if(!b.alive) return;
    if(b.type==BLK_BRICK){
        drawBrickBlock(b.x,b.y,b.col);
    } else if(b.type==BLK_STEEL){
        drawSteelBlock(b.x,b.y);
    } else {
        glColor3f(b.col.r,b.col.g,b.col.b);
        fillRect(b.x,b.y,BW,BH);
        glColor3f(fminf(1.0f,b.col.r+0.35f),fminf(1.0f,b.col.g+0.35f),fminf(1.0f,b.col.b+0.35f));
        fillRect(b.x+2,b.y+BH-4,BW-4,3);
        glColor3f(b.col.r*0.45f,b.col.g*0.45f,b.col.b*0.45f);
        fillRect(b.x+2,b.y+1,BW-4,2);
    }
    glColor3f(0,0,0);
    lineRect(b.x,b.y,BW,BH);
    if(b.hp>1 && b.type!=BLK_STEEL){
        char buf[4]; sprintf(buf,"%d",b.hp);
        glColor3f(1,1,1);
        drawStr(b.x+BW/2.0f-3,b.y+BH/2.0f-5,buf,GLUT_BITMAP_HELVETICA_10);
    }
}

// ---------- PADDLE ----------
void drawPaddle(){
    float px=padX-padW/2.0f;
    glColor3f(0.0f,0.1f,0.25f);
    fillRect(px+3,PAD_Y-3,padW,PAD_H);
    glBegin(GL_QUADS);
    glColor3f(0.1f,0.35f,0.9f); glVertex2f(px,    PAD_Y);
    glColor3f(0.1f,0.35f,0.9f); glVertex2f(px+padW,PAD_Y);
    glColor3f(0.45f,0.72f,1.0f);glVertex2f(px+padW,PAD_Y+PAD_H);
    glColor3f(0.45f,0.72f,1.0f);glVertex2f(px,    PAD_Y+PAD_H);
    glEnd();
    glColor3f(0.72f,0.92f,1.0f);
    fillRect(px+4,PAD_Y+PAD_H-3,padW-8,2);
    glColor3f(0.3f,0.65f,1.0f);
    lineRect(px,PAD_Y,padW,PAD_H,1.5f);
    if(shootTimer>0){
        glColor3f(1.0f,1.0f,0.3f);
        fillRect(px,     PAD_Y+PAD_H,  5, 8);
        fillRect(px+padW-5,PAD_Y+PAD_H,5, 8);
    }
}

// ---------- BALL ----------
void drawBallObj(){
    if(ball.fire){
        glColor3f(1.0f,0.35f,0.0f);
        circ(ball.x,ball.y,BALL_R+6);
        glColor3f(1.0f,0.72f,0.0f);
        circ(ball.x,ball.y,BALL_R+3);
    } else {
        glColor3f(0.45f,0.45f,0.65f);
        circ(ball.x,ball.y,BALL_R+2);
    }
    glColor3f(0.92f,0.92f,1.0f);
    circ(ball.x,ball.y,BALL_R);
    glColor3f(1,1,1);
    circ(ball.x+BALL_R*0.35f, ball.y+BALL_R*0.35f, BALL_R*0.27f);
}

// ---------- PERKS ----------
void drawPerks(){
    const char* perkLabel[PK_COUNT]={"LIFE","+WIDE","+FAST","FIRE","SHOOT","SLOW","SHRINK","DEATH"};
    Col3 perkCol[PK_COUNT]={
        {0.2f,1.0f,0.2f}, {0.0f,0.8f,1.0f}, {1.0f,0.6f,0.0f}, {1.0f,0.35f,0.0f},
        {1.0f,1.0f,0.0f}, {0.4f,0.4f,1.0f}, {1.0f,0.0f,0.6f}, {0.15f,0.15f,0.15f}
    };
    for(auto& p:perks){
        if(!p.alive) continue;
        Col3 c=perkCol[p.type];
        glColor3f(c.r*0.3f,c.g*0.3f,c.b*0.3f);
        fillRect(p.x-17,p.y-11,34,22);
        glColor3f(c.r,c.g,c.b);
        fillRect(p.x-15,p.y-9,30,18);
        glColor3f(0,0,0);
        int len=glutBitmapLength(GLUT_BITMAP_HELVETICA_10,(const unsigned char*)perkLabel[p.type]);
        drawStr(p.x-len/2.0f, p.y-4, perkLabel[p.type], GLUT_BITMAP_HELVETICA_10);
        glColor3f(1,1,1);
        lineRect(p.x-15,p.y-9,30,18,1.2f);
    }
}

// ---------- BULLETS ----------
void drawBullets(){
    for(auto& b:bullets){
        if(!b.alive) continue;
        glColor3f(1.0f,0.8f,0.0f);
        fillRect(b.x-2.5f,b.y-1,5,12);
        glColor3f(1.0f,1.0f,0.6f);
        fillRect(b.x-1.5f,b.y,3,10);
    }
}

// ---------- HUD ----------
void drawHUD(){
    glColor3f(0.06f,0.06f,0.18f);
    fillRect(0,(float)PLAY_H,(float)W,(float)HUD_H);
    glColor3f(0.2f,0.3f,0.8f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(0,(float)PLAY_H); glVertex2f((float)W,(float)PLAY_H);
    glEnd();
    glLineWidth(1);

    char buf[80];

    glColor3f(1.0f,0.9f,0.0f);
    sprintf(buf,"SCORE: %d",score);
    drawStr(10,(float)PLAY_H+11,buf,GLUT_BITMAP_HELVETICA_12);

    // Hearts
    for(int i=0;i<lives && i<6;i++){
        glColor3f(1.0f,0.15f,0.15f);
        circ(195.0f+i*22.0f,(float)PLAY_H+17,8);
        glColor3f(1.0f,0.5f,0.5f);
        circ(193.0f+i*22.0f,(float)PLAY_H+19,3);
    }
    glColor3f(1.0f,0.25f,0.25f);
    sprintf(buf,"x%d",lives);
    drawStr(195.0f+fminf(6,lives)*22.0f,(float)PLAY_H+11,buf,GLUT_BITMAP_HELVETICA_10);

    glColor3f(0.25f,1.0f,0.5f);
    sprintf(buf,"TIME: %.1fs",elapsed);
    drawStr(380,(float)PLAY_H+11,buf,GLUT_BITMAP_HELVETICA_12);

    glColor3f(1.0f,0.6f,0.0f);
    sprintf(buf,"LEVEL: %d/3",level);
    drawStr(540,(float)PLAY_H+11,buf,GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.7f,0.7f,0.9f);
    sprintf(buf,"SPD:%.1fx",speedMult);
    drawStr(680,(float)PLAY_H+11,buf,GLUT_BITMAP_HELVETICA_10);

    // Perk badges
    float px2=8;
    if(fireTimer>0){
        glColor3f(1.0f,0.5f,0.0f);
        sprintf(buf,"FIRE %.0fs",fireTimer);
        drawStr(px2,(float)PLAY_H-15,buf,GLUT_BITMAP_HELVETICA_10);
        px2+=80;
    }
    if(shootTimer>0){
        glColor3f(1,1,0);
        sprintf(buf,"SHOOT %.0fs",shootTimer);
        drawStr(px2,(float)PLAY_H-15,buf,GLUT_BITMAP_HELVETICA_10);
    }

    if(!ball.active){
        glColor3f(0.7f,1.0f,0.7f);
        drawStr((float)W/2-130,(float)PLAY_H/2+12,
                "Press SPACE or left-click to launch!",GLUT_BITMAP_HELVETICA_12);
    }
}

// ---------- MENU ----------
void drawMenuButton(const char* txt,float cx,float cy,float bw,float bh,bool sel){
    if(sel){
        glColor3f(0.9f,0.75f,0.0f);
        fillRect(cx-bw/2-4,cy-bh/2-4,bw+8,bh+8);
        glColor3f(0.12f,0.06f,0.28f);
        fillRect(cx-bw/2,cy-bh/2,bw,bh);
        glColor3f(1.0f,0.95f,0.3f);
    } else {
        glColor3f(0.08f,0.08f,0.22f);
        fillRect(cx-bw/2,cy-bh/2,bw,bh);
        glColor3f(0.35f,0.45f,0.7f);
        lineRect(cx-bw/2,cy-bh/2,bw,bh,1.5f);
        glColor3f(0.78f,0.82f,1.0f);
    }
    int len=glutBitmapLength(GLUT_BITMAP_HELVETICA_18,(const unsigned char*)txt);
    drawStr(cx-len/2.0f,cy-7,txt,GLUT_BITMAP_HELVETICA_18);
}

void drawMenu(){
    glColor3f(0.02f,0.02f,0.1f);
    fillRect(0,0,(float)W,(float)H);
    drawStars();

    glColor3f(0.2f,0.06f,0.5f);
    fillRect((float)W/2-180,(float)H-155,360,85);
    glColor3f(0.7f,0.3f,1.0f);
    lineRect((float)W/2-180,(float)H-155,360,85,2);
    glColor3f(1.0f,0.88f,0.0f);
    int tlen=glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,(const unsigned char*)"DX  BALL");
    drawStr((float)W/2-tlen/2.0f,(float)H-120,"DX  BALL",GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.6f,0.82f,1.0f);
    drawStr((float)W/2-75,(float)H-150,"Made by Yeamin and Shara",GLUT_BITMAP_HELVETICA_12);

    const char* items[]={"START GAME","HIGH SCORES","HELP","EXIT"};
    for(int i=0;i<4;i++)
        drawMenuButton(items[i],(float)W/2,(float)H/2+55.0f-i*65.0f,230,44,(menuSel==i));

    glColor3f(0.38f,0.4f,0.62f);
    drawStr((float)W/2-175,22,"Arrow Keys to navigate   |   Enter or click to select",GLUT_BITMAP_HELVETICA_10);
}

// ---------- PAUSE ----------
void drawPause(){
    glColor3f(0.0f,0.0f,0.05f);
    fillRect(0,0,(float)W,(float)H);
    glColor3f(0.1f,0.04f,0.22f);
    fillRect((float)W/2-170,(float)H/2-80,340,160);
    glColor3f(0.65f,0.35f,1.0f);
    lineRect((float)W/2-170,(float)H/2-80,340,160,2.5f);
    glColor3f(1.0f,0.9f,0.0f);
    int pl=glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,(const unsigned char*)"PAUSED");
    drawStr((float)W/2-pl/2.0f,(float)H/2+28,"PAUSED",GLUT_BITMAP_TIMES_ROMAN_24);
    glColor3f(0.8f,0.82f,1.0f);
    drawStr((float)W/2-105,(float)H/2-15,"P   - Resume",GLUT_BITMAP_HELVETICA_12);
    drawStr((float)W/2-105,(float)H/2-38,"ESC - Return to Main Menu",GLUT_BITMAP_HELVETICA_12);
}

// ---------- GAME OVER ----------
void drawGameOver(){
    glColor3f(0.02f,0.0f,0.04f);
    fillRect(0,0,(float)W,(float)H);
    drawStars();
    glColor3f(1.0f,0.12f,0.12f);
    int gl=glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,(const unsigned char*)"GAME  OVER");
    drawStr((float)W/2-gl/2.0f,(float)H/2+90,"GAME  OVER",GLUT_BITMAP_TIMES_ROMAN_24);
    char buf[80];
    glColor3f(1.0f,0.9f,0.0f);
    sprintf(buf,"Score : %d",score);
    drawStr((float)W/2-80,(float)H/2+40,buf,GLUT_BITMAP_HELVETICA_18);
    sprintf(buf,"Level reached : %d",level);
    drawStr((float)W/2-80,(float)H/2+10,buf,GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.65f,0.65f,0.75f);
    drawStr((float)W/2-130,(float)H/2-45,"Press ENTER or ESC to return to Menu",GLUT_BITMAP_HELVETICA_12);
}

// ---------- WIN ----------
void drawWin(){
    glColor3f(0.0f,0.04f,0.02f);
    fillRect(0,0,(float)W,(float)H);
    drawStars();
    glColor3f(0.15f,1.0f,0.5f);
    int wl=glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,(const unsigned char*)"YOU  WIN !");
    drawStr((float)W/2-wl/2.0f,(float)H/2+90,"YOU  WIN !",GLUT_BITMAP_TIMES_ROMAN_24);
    char buf[80];
    glColor3f(1.0f,0.9f,0.0f);
    sprintf(buf,"Final Score : %d",score);
    drawStr((float)W/2-80,(float)H/2+40,buf,GLUT_BITMAP_HELVETICA_18);
    sprintf(buf,"Time : %.1f seconds",elapsed);
    drawStr((float)W/2-80,(float)H/2+10,buf,GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.65f,0.65f,0.75f);
    drawStr((float)W/2-130,(float)H/2-45,"Press ENTER or ESC to return to Menu",GLUT_BITMAP_HELVETICA_12);
}

// ---------- HELP ----------
void drawHelpScreen(){
    glColor3f(0.02f,0.02f,0.1f);
    fillRect(0,0,(float)W,(float)H);
    drawStars();

    glColor3f(1.0f,0.88f,0.0f);
    drawStr((float)W/2-25,(float)H-48,"HELP",GLUT_BITMAP_TIMES_ROMAN_24);

    struct HLine { const char* txt; float r,g,b; };
    const HLine lines[]={
        {"-- CONTROLS --",                          1.0f,0.7f,0.0f},
        {"Left / Right Arrows  :  Move paddle",     0.9f,0.9f,0.9f},
        {"Mouse move           :  Move paddle",     0.9f,0.9f,0.9f},
        {"SPACE / Left-click   :  Launch ball",     0.9f,0.9f,0.9f},
        {"Left-click (SHOOT)   :  Fire bullets",    0.9f,0.9f,0.9f},
        {"P                    :  Pause / Resume",  0.9f,0.9f,0.9f},
        {"ESC                  :  Back to menu",    0.9f,0.9f,0.9f},
        {"",                                        0,0,0},
        {"-- BLOCKS --",                            1.0f,0.7f,0.0f},
        {"Normal (1 hit)   :  coloured blocks",     0.85f,0.85f,0.85f},
        {"Hard   (2 hits)  :  darker colours",      0.85f,0.85f,0.85f},
        {"Brick  (3 hits)  :  brown brick pattern", 0.85f,0.85f,0.85f},
        {"Steel  (unbreak) :  grey X block",        0.85f,0.85f,0.85f},
        {"",                                        0,0,0},
        {"-- PERKS (catch the falling capsule!) --",1.0f,0.7f,0.0f},
        {"LIFE   (green)   :  +1 Life",             0.2f,1.0f,0.2f},
        {"+WIDE  (cyan)    :  Wider paddle",        0.0f,0.8f,1.0f},
        {"+FAST  (orange)  :  Faster ball",         1.0f,0.6f,0.0f},
        {"FIRE   (red)     :  Fireball  (12 s)",    1.0f,0.35f,0.0f},
        {"SHOOT  (yellow)  :  Bullet paddle (15 s)",1.0f,1.0f,0.0f},
        {"SLOW   (blue)    :  Slower ball",         0.4f,0.4f,1.0f},
        {"",                                        0,0,0},
        {"-- DANGERS --",                           1.0f,0.3f,0.3f},
        {"SHRINK (pink)    :  Paddle shrinks!",     1.0f,0.0f,0.6f},
        {"DEATH  (dark)    :  Lose a life now!",    0.8f,0.2f,0.2f},
        {"",                                        0,0,0},
        {"Press ESC to return to menu",             0.5f,0.5f,0.7f},
    };
    int n=(int)(sizeof(lines)/sizeof(lines[0]));
    for(int i=0;i<n;i++){
        glColor3f(lines[i].r,lines[i].g,lines[i].b);
        drawStr(55,(float)H-82-(float)i*19,lines[i].txt,GLUT_BITMAP_HELVETICA_12);
    }
}

// ---------- HIGH SCORES ----------
void drawHighScoreScreen(){
    glColor3f(0.02f,0.02f,0.1f);
    fillRect(0,0,(float)W,(float)H);
    drawStars();

    glColor3f(1.0f,0.88f,0.0f);
    int tl=glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,(const unsigned char*)"HIGH  SCORES");
    drawStr((float)W/2-tl/2.0f,(float)H-55,"HIGH  SCORES",GLUT_BITMAP_TIMES_ROMAN_24);

    Col3 medals[]={{1.0f,0.85f,0.0f},{0.8f,0.8f,0.8f},{0.78f,0.5f,0.2f},{0.6f,0.6f,0.7f},{0.55f,0.55f,0.65f}};
    if(highScores.empty()){
        glColor3f(0.55f,0.55f,0.7f);
        drawStr((float)W/2-130,(float)H/2,"No scores yet — play a game first!",GLUT_BITMAP_HELVETICA_12);
    } else {
        for(int i=0;i<(int)highScores.size();i++){
            auto& h=highScores[i];
            char buf[100];
            sprintf(buf,"#%d   Score: %5d   Level: %d   Time: %.1fs",i+1,h.score,h.level,h.time);
            glColor3f(medals[i].r,medals[i].g,medals[i].b);
            int bl=glutBitmapLength(GLUT_BITMAP_HELVETICA_18,(const unsigned char*)buf);
            drawStr((float)W/2-bl/2.0f,(float)H/2+80.0f-i*50,buf,GLUT_BITMAP_HELVETICA_18);
        }
    }
    glColor3f(0.45f,0.45f,0.65f);
    drawStr((float)W/2-110,28,"Press ESC to return to menu",GLUT_BITMAP_HELVETICA_12);
}

// ---------- MAIN DISPLAY DISPATCHER ----------
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    switch(gState){
    case MENU:          drawMenu(); break;
    case HELP_SCR:      drawHelpScreen(); break;
    case HIGHSCORE_SCR: drawHighScoreScreen(); break;
    case PLAYING:
    case PAUSED:
        glColor3f(0.02f,0.02f,0.08f);
        fillRect(0,0,(float)W,(float)PLAY_H);
        drawStars();
        drawParticles();    // <-- This is now defined
        for(auto& b:blocks) drawBlock(b);
        drawPerks();
        drawBullets();
        drawPaddle();
        drawBallObj();
        drawHUD();
        if(gState==PAUSED) drawPause();
        break;
    case GAMEOVER: drawGameOver(); break;
    case WIN:      drawWin(); break;
    }
    glutSwapBuffers();
}

