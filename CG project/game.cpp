
#include <cstdlib>
#include <algorithm>
#include "game.h"
#include "globals.h"

// ---------- STARFIELD ----------
void initStars(){
    stars.clear();
    for(int i=0;i<150;i++){
        Star s;
        s.x=(float)(rand()%W);
        s.y=(float)(rand()%H);
        s.bright=0.25f+(rand()%75)/100.0f;
        stars.push_back(s);
    }
}

// ---------- PARTICLES ----------
void spawnParticles(float x,float y,Col3 col,int n){
    for(int i=0;i<n;i++){
        float a=2.0f*(float)M_PI*rand()/RAND_MAX;
        float sp=50.0f+rand()%120;
        Particle p;
        p.x=x; p.y=y;
        p.vx=sp*cosf(a); p.vy=sp*sinf(a);
        p.life=p.maxLife=0.35f+0.4f*(float)rand()/RAND_MAX;
        p.col=col;
        particles.push_back(p);
    }
}

void updateParticles(float dt){
    for(auto&p:particles){
        p.life-=dt;
        p.x+=p.vx*dt;
        p.y+=p.vy*dt;
        p.vy-=80.0f*dt;
    }
    particles.erase(
        std::remove_if(particles.begin(),particles.end(),[](const Particle&p){return p.life<=0;}),
        particles.end()
    );
}

// ---------- LEVEL SETUP ----------
void setupLevel(int lvl){
    blocks.clear(); perks.clear(); bullets.clear(); particles.clear();
    broken=0; totalBreak=0;

    for(int row=0;row<ROWS;row++){
        for(int col=0;col<COLS;col++){
            Block b;
            b.x = BSTART_X + col*(BW+BG);
            b.y = BSTART_Y - row*(BH+BG);
            b.alive=true;

            int r=rand()%100;
            int steelP  = (lvl-1)*4;           //  0 / 4 / 8
            int brickP  = 10+(lvl-1)*6;         // 10/16/22
            int hardP   = 15+(lvl-1)*5;         // 15/20/25

            if     (r<steelP)             { b.type=BLK_STEEL;  b.hp=999; }
            else if(r<steelP+brickP)      { b.type=BLK_BRICK;  b.hp=3;   }
            else if(r<steelP+brickP+hardP){ b.type=BLK_HARD;   b.hp=2;   }
            else                          { b.type=BLK_NORMAL; b.hp=1;   }

            int ci=rand()%3;
            // Block colour palette defined in render.cpp, but we use the same logic
            switch(b.type){
                case BLK_NORMAL:
                    b.col = (ci==0)?Col3{1.0f,0.35f,0.35f} :
                            (ci==1)?Col3{0.35f,1.0f,0.45f} :
                                     Col3{0.35f,0.55f,1.0f};
                    break;
                case BLK_HARD:
                    b.col = (ci==0)?Col3{0.85f,0.1f,0.1f} :
                            (ci==1)?Col3{0.1f,0.75f,0.2f} :
                                     Col3{0.15f,0.25f,0.85f};
                    break;
                case BLK_BRICK:
                    b.col = (ci==0)?Col3{0.78f,0.38f,0.18f} :
                            (ci==1)?Col3{0.68f,0.3f,0.12f} :
                                     Col3{0.58f,0.25f,0.08f};
                    break;
                case BLK_STEEL:
                    b.col = Col3{0.6f,0.6f,0.65f};
                    break;
            }

            if(b.type!=BLK_STEEL) totalBreak++;
            blocks.push_back(b);
        }
    }
}

// ---------- BALL / PADDLE RESET ----------
void resetBall(bool waitForLaunch){
    ball.x  = padX;
    ball.y  = PAD_Y + PAD_H + BALL_R + 2.0f;
    float ang = (float)M_PI/2.0f + ((rand()%61)-30)*((float)M_PI/180.0f);
    float sp  = BALL_SPEED * speedMult;
    ball.vx   = sp * cosf(ang);
    ball.vy   = sp * sinf(ang);
    ball.active = !waitForLaunch;
    ball.fire   = false;
}

void initGame(){
    padX=W/2.0f; padW=PAD_DEF_W;
    lives=3; score=0; level=1; elapsed=0.0f; speedMult=1.0f;
    fireTimer=0; shootTimer=0; shootCD=0;
    leftKey=false; rightKey=false;
    setupLevel(level);
    resetBall(true);
}

// ---------- PERKS ----------
void applyPerk(PerkType t){
    switch(t){
    case PK_LIFE:
        lives++;
        score+=100;
        break;
    case PK_WIDE:
        padW=fminf(210.0f, padW*1.4f);
        break;
    case PK_FAST:
        ball.vx*=1.25f; ball.vy*=1.25f;
        speedMult*=1.25f;
        break;
    case PK_FIRE:
        fireTimer=12.0f;
        ball.fire=true;
        score+=30;
        break;
    case PK_SHOOT:
        shootTimer=15.0f;
        score+=30;
        break;
    case PK_SLOW:
        ball.vx*=0.75f; ball.vy*=0.75f;
        speedMult=fmaxf(0.55f, speedMult*0.75f);
        break;
    case PK_SHRINK:
        padW=fmaxf(38.0f, padW*0.6f);
        break;
    case PK_DEATH:
        lives--;
        if(lives<=0){ lives=0; gState=GAMEOVER; }
        break;
    default: break;
    }
}

void spawnPerk(float x,float y){
    int r=rand()%100;
    PerkType t;
    if     (r<18) t=PK_LIFE;
    else if(r<33) t=PK_WIDE;
    else if(r<46) t=PK_FAST;
    else if(r<57) t=PK_FIRE;
    else if(r<67) t=PK_SHOOT;
    else if(r<77) t=PK_SLOW;
    else if(r<90) t=PK_SHRINK;
    else          t=PK_DEATH;
    perks.push_back({x,y,t,true});
}

// ---------- COLLISION ----------
bool circleAABB(float bx,float by,float br,
                float rx,float ry,float rw,float rh,
                float& nx,float& ny)
{
    float cx=fmaxf(rx,fminf(bx,rx+rw));
    float cy=fmaxf(ry,fminf(by,ry+rh));
    float dx=bx-cx, dy=by-cy;
    float d2=dx*dx+dy*dy;
    if(d2>=br*br) return false;
    float d=sqrtf(d2);
    if(d<0.001f){ nx=0.0f; ny=1.0f; }
    else { nx=dx/d; ny=dy/d; }
    return true;
}

void reflectVel(float& vx,float& vy,float nx,float ny){
    float dot=vx*nx+vy*ny;
    vx-=2.0f*dot*nx;
    vy-=2.0f*dot*ny;
}

// ---------- UPDATE ----------
void update(float dt){
    if(gState!=PLAYING) return;

    elapsed+=dt;

    // auto speed ramp every 25 s
    float newMult = 1.0f + (int)(elapsed/25.0f)*0.15f;
    if(newMult>speedMult){
        float ratio=newMult/speedMult;
        ball.vx*=ratio; ball.vy*=ratio;
        speedMult=newMult;
    }

    // perk timers
    if(fireTimer>0){ fireTimer-=dt; if(fireTimer<=0){fireTimer=0;ball.fire=false;} }
    if(shootTimer>0){ shootTimer-=dt; if(shootTimer<=0) shootTimer=0; }
    if(shootCD>0) shootCD-=dt;

    // paddle movement
    if(leftKey)  padX-=PAD_SPEED*dt;
    if(rightKey) padX+=PAD_SPEED*dt;
    padX=fmaxf(padW/2.0f, fminf((float)W-padW/2.0f, padX));
    if(!ball.active){ ball.x=padX; ball.y=PAD_Y+PAD_H+BALL_R+2.0f; }

    updateParticles(dt);

    if(!ball.active) return;

    // move ball
    ball.x+=ball.vx*dt;
    ball.y+=ball.vy*dt;

    // wall bounces
    if(ball.x-BALL_R<0)          { ball.x=BALL_R;        if(ball.vx<0) ball.vx=-ball.vx; }
    if(ball.x+BALL_R>(float)W)   { ball.x=(float)W-BALL_R; if(ball.vx>0) ball.vx=-ball.vx; }
    if(ball.y+BALL_R>(float)PLAY_H){ ball.y=(float)PLAY_H-BALL_R; if(ball.vy>0) ball.vy=-ball.vy; }

    // paddle collision
    if(ball.vy<0.0f){
        float px=padX-padW/2.0f;
        float nx,ny;
        if(circleAABB(ball.x,ball.y,BALL_R, px,PAD_Y,padW,PAD_H, nx,ny)){
            float rel=(ball.x-padX)/(padW/2.0f);
            rel=fmaxf(-0.9f,fminf(0.9f,rel));
            float ang=(float)M_PI/2.0f - rel*((float)M_PI/3.0f);
            float sp=sqrtf(ball.vx*ball.vx+ball.vy*ball.vy);
            ball.vx=sp*cosf(ang);
            ball.vy=sp*sinf(ang);
            ball.y=PAD_Y+PAD_H+BALL_R+1.0f;
        }
    }

    // ball lost
    if(ball.y-BALL_R<0.0f){
        lives--;
        if(lives<=0){ lives=0; gState=GAMEOVER; }
        else resetBall(true);
        return;
    }

    // block collisions
    for(auto& b:blocks){
        if(!b.alive) continue;
        float nx,ny;
        if(circleAABB(ball.x,ball.y,BALL_R, b.x,b.y,BW,BH, nx,ny)){
            if(!ball.fire) reflectVel(ball.vx,ball.vy,nx,ny);
            if(b.type!=BLK_STEEL){
                b.hp--;
                if(b.hp<=0){
                    b.alive=false; broken++;
                    int pts=(b.type==BLK_BRICK)?30:(b.type==BLK_HARD)?20:10;
                    score+=pts;
                    spawnParticles(b.x+BW/2.0f, b.y+BH/2.0f, b.col, 10);
                    if(rand()%100<30) spawnPerk(b.x+BW/2.0f, b.y);
                } else {
                    b.col.r*=0.72f; b.col.g*=0.72f; b.col.b*=0.72f;
                }
            }
        }
    }

    // level complete
    if(broken>=totalBreak){
        int bonus=fmaxf(0.0f, 3000.0f - elapsed*8.0f);
        score+=bonus;
        HSEntry he={score,level,elapsed};
        highScores.push_back(he);
        std::sort(highScores.begin(),highScores.end(),
                  [](const HSEntry&a,const HSEntry&b){return a.score>b.score;});
        if((int)highScores.size()>5) highScores.resize(5);

        level++;
        if(level>3){ gState=WIN; }
        else {
            setupLevel(level);
            resetBall(true);
        }
        return;
    }

    // move perks
    for(auto& p:perks){
        if(!p.alive) continue;
        p.y-=95.0f*dt;
        if(p.y<PAD_Y+PAD_H && p.y>PAD_Y-28.0f &&
           p.x>padX-padW/2.0f-15.0f && p.x<padX+padW/2.0f+15.0f){
            applyPerk(p.type); p.alive=false;
        }
        if(p.y<-25.0f) p.alive=false;
    }
    perks.erase(std::remove_if(perks.begin(),perks.end(),
                [](const Perk&p){return !p.alive;}),perks.end());

    // bullets
    for(auto& b:bullets){
        if(!b.alive) continue;
        b.y+=400.0f*dt;
        if(b.y>(float)PLAY_H){ b.alive=false; continue; }
        for(auto& bl:blocks){
            if(!bl.alive) continue;
            if(b.x>=bl.x && b.x<=bl.x+BW && b.y>=bl.y && b.y<=bl.y+BH){
                if(bl.type!=BLK_STEEL){
                    bl.hp--;
                    if(bl.hp<=0){
                        bl.alive=false; broken++;
                        spawnParticles(bl.x+BW/2.0f,bl.y+BH/2.0f,bl.col,6);
                        score+=10;
                    }
                }
                b.alive=false;
                break;
            }
        }
    }
    bullets.erase(std::remove_if(bullets.begin(),bullets.end(),
                   [](const Bullet&b){return !b.alive;}),bullets.end());
}

void fireBullets(){
    if(shootTimer>0 && shootCD<=0){
        bullets.push_back({padX-padW/2.0f+2.0f, PAD_Y+PAD_H, true});
        bullets.push_back({padX+padW/2.0f-2.0f, PAD_Y+PAD_H, true});
        shootCD=0.22f;
    }
}
