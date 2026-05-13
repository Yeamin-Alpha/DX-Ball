#include <GL/freeglut.h>
#include "input.h"
#include "game.h"
#include "globals.h"

void keyDown(unsigned char k,int,int){
    if(gState==PLAYING){
        if(k==' '){ ball.active=true; fireBullets(); }
        if(k=='p'||k=='P') gState=PAUSED;
        if(k==27)           gState=MENU;
    }
    else if(gState==PAUSED){
        if(k=='p'||k=='P') gState=PLAYING;
        if(k==27)           gState=MENU;
    }
    else if(gState==MENU){
        if(k==13){
            switch(menuSel){
            case 0: initGame(); gState=PLAYING;      break;
            case 1: gState=HIGHSCORE_SCR;            break;
            case 2: gState=HELP_SCR;                 break;
            case 3: exit(0);
            }
        }
        if(k==27) exit(0);
    }
    else if(gState==HELP_SCR || gState==HIGHSCORE_SCR){
        if(k==27) gState=MENU;
    }
    else if(gState==GAMEOVER || gState==WIN){
        if(k==13||k==27) gState=MENU;
    }
}

void specialDown(int k,int,int){
    if(gState==MENU){
        if(k==GLUT_KEY_UP)   menuSel=(menuSel-1+4)%4;
        if(k==GLUT_KEY_DOWN) menuSel=(menuSel+1)%4;
        return;
    }
    if(k==GLUT_KEY_LEFT)  leftKey=true;
    if(k==GLUT_KEY_RIGHT) rightKey=true;
}

void specialUp(int k,int,int){
    if(k==GLUT_KEY_LEFT)  leftKey=false;
    if(k==GLUT_KEY_RIGHT) rightKey=false;
}

void mouseMove(int x,int y){
    if(gState!=PLAYING) return;
    (void)y;
    padX=(float)x;
    padX=fmaxf(padW/2.0f, fminf((float)W-padW/2.0f, padX));
    if(!ball.active){ ball.x=padX; ball.y=PAD_Y+PAD_H+BALL_R+2.0f; }
}

void mouseClick(int btn,int st,int x,int y){
    if(gState==PLAYING && btn==GLUT_LEFT_BUTTON && st==GLUT_DOWN){
        ball.active=true;
        fireBullets();
    }
    if(gState==MENU && btn==GLUT_LEFT_BUTTON && st==GLUT_DOWN){
        float fy=(float)(H-y);
        for(int i=0;i<4;i++){
            float cy=(float)H/2.0f+55.0f-i*65.0f;
            if(fabs(fy-cy)<25 && fabs((float)x-(float)W/2)<115){
                menuSel=i;
                switch(i){
                case 0: initGame(); gState=PLAYING;    break;
                case 1: gState=HIGHSCORE_SCR;          break;
                case 2: gState=HELP_SCR;               break;
                case 3: exit(0);
                }
            }
        }
    }
    if((gState==HELP_SCR||gState==HIGHSCORE_SCR||gState==GAMEOVER||gState==WIN)
       && btn==GLUT_LEFT_BUTTON && st==GLUT_DOWN){
        gState=MENU;
    }
}

