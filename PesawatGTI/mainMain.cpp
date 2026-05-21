#include "Game.h"

void reshape(int w, int h){
    if(h==0) h=1;
    glViewport(0,0,w,h);
}

void keyDown(unsigned char k, int /*x*/, int /*y*/){
    keyState[(unsigned char)k] = true;
    if(k==27){ exit(0); }
    if(k=='p'||k=='P'){ gamePaused=!gamePaused; }
    if(k=='c'||k=='C'){ camMode=(camMode+1)%2; }
    if(k=='t'||k=='T'){ aimAssistOn=!aimAssistOn; }
    if(k=='f'||k=='F'){
        int best = findBestLockTarget();
        if(best >= 0) lockedEnemy = (lockedEnemy==best) ? -1 : best; 
    }
    if(k=='x'||k=='X'){ fireMissile(); }
    if((k=='r'||k=='R') && gameOver){
        gameOver = false; score=0; enemiesKilled=0; gameTime=0; craterCount=0; lockedEnemy=-1; lockOnTimer=0;
        initPlayer(); initEnemies(); initBullets(); initParticles(); initBuildings();
    }
}

void keyUp(unsigned char k, int /*x*/, int /*y*/){ keyState[(unsigned char)k] = false; }
void specialDown(int k, int /*x*/, int /*y*/){ specState[k]=true; }
void specialUp(int k, int /*x*/, int /*y*/){ specState[k]=false; }

void mouseClick(int btn, int state, int x, int y){
    if(btn==GLUT_LEFT_BUTTON){
        mouseBtn[0] = (state==GLUT_DOWN)?1:0;
        if(state==GLUT_DOWN && !gameOver && !gamePaused && player.alive && player.shootCooldown<=0){
            fireBullet(player,true); player.shootCooldown=0.18f;
        }
    }
    if(btn==GLUT_RIGHT_BUTTON) mouseBtn[1]=(state==GLUT_DOWN)?1:0;
    if(btn==3 && state==GLUT_DOWN){ camOrbitDist-=2.f; if(camOrbitDist<5) camOrbitDist=5; }
    if(btn==4 && state==GLUT_DOWN){ camOrbitDist+=2.f; if(camOrbitDist>60) camOrbitDist=60; }
    lastMouseX=x; lastMouseY=y;
}

void mouseMotion(int x, int y){
    if(camMode==1 && mouseBtn[1]){
        camOrbitYaw   += (x-lastMouseX)*0.5f;
        camOrbitPitch -= (y-lastMouseY)*0.5f;
        if(camOrbitPitch>80) camOrbitPitch=80; if(camOrbitPitch<5)  camOrbitPitch=5;
    }
    lastMouseX=x; lastMouseY=y;
}

void update(int val){
    if(!gamePaused && !gameOver){
        int now = glutGet(GLUT_ELAPSED_TIME);
        dt = (now - lastTime)*0.001f; if(dt > 0.05f) dt=0.05f;
        lastTime = now; gameTime += dt; propAngle += 720.f*dt;

        lockOnTimer -= dt;
        if(lockOnTimer <= 0.f){
            lockOnTimer = 0.3f;
            if(lockedEnemy < 0 || !enemies[lockedEnemy].alive || vlen(enemies[lockedEnemy].pos - player.pos) > 130.f){
                lockedEnemy = findBestLockTarget();
            } else {
                int better = findBestLockTarget();
                if(better >= 0) lockedEnemy = better;
            }
        }

        updatePlayer(dt);
        for(int i=0;i<ENEMY_COUNT;i++) updateEnemy(enemies[i],dt);

        if(player.alive){
            bool crashed = false;
            if(player.pos.y <= 2.5f) crashed = true;
            if(!crashed){
                for(int i=0; i<BUILDING_COUNT; i++){
                    Building& b = buildings[i];
                    if(sphereAABB(player.pos, 1.2f, b.x, b.z, b.w, b.d, b.h)){
                        crashed = true; b.crackLevel = 2; b.damaged = true;
                        addCrater(player.pos, i, true); break;
                    }
                }
            }
            if(crashed){
                player.health = 0; player.alive = false; gameOver = true; spawnParticles(player.pos, 80, true);
            }
        }

        challengeTimer -= dt;
        if(challengeTimer <= 0.f){ challengeTimer = 60.f; triggerChallenge(); }
        if(challengePopup){ challengePopupTimer -= dt; if(challengePopupTimer <= 0) challengePopup=false; }

        if(missileAmmo < 5 && missileReload > 0){
            missileReload -= dt;
            if(missileReload <= 0){ missileAmmo++; if(missileAmmo < 5) missileReload = MISSILE_RELOAD_TIME; }
        }

        updateMissiles(dt);

        for(int i=0;i<BULLET_MAX;i++){
            if(!bullets[i].active) continue;
            bullets[i].pos = bullets[i].pos + bullets[i].vel*dt; bullets[i].life -= dt;
            if(bullets[i].life <= 0){ bullets[i].active=false; continue; }
            checkBulletBuildingCollision(bullets[i]);
            checkBulletPlaneCollision(bullets[i]);
        }

        for(int i=0;i<PARTICLE_MAX;i++){
            if(!particles[i].active) continue;
            particles[i].pos = particles[i].pos + particles[i].vel*dt;
            particles[i].vel.y -= 4.f*dt; particles[i].life -= dt;
            particles[i].a = particles[i].life / 1.2f;
            if(particles[i].life <= 0) particles[i].active=false;
        }

        for(int i=0;i<craterCount;i++){
            if(!craters[i].onFire) continue;
            craters[i].fireTimer -= dt;
            if(craters[i].fireTimer <= 0){ craters[i].fireTimer = randf(0.1f,0.3f); spawnParticles(craters[i].pos, 2, true); }
        }

        bool allDead=true;
        for(int i=0;i<ENEMY_COUNT;i++) if(enemies[i].alive){allDead=false;break;}
        if(allDead && !gameOver){
            initEnemies(); waveCount++; scoreFromWaves += 200 + waveCount*50;
            score = scoreFromKills + scoreFromWaves + scoreFromBldg;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void initGL(){
    glClearColor(0.25f,0.52f,0.95f,1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  difLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spcLight);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_FOG); glFogi(GL_FOG_MODE, GL_LINEAR);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setupTextures();
    buildDisplayLists();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Airplane City Shooter - OpenGL GLUT");

    initGL();
    initBuildings(); initTrees(); initPlayer(); initEnemies();
    initBullets(); initParticles();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown); glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown); glutSpecialUpFunc(specialUp);
    glutMouseFunc(mouseClick); glutMotionFunc(mouseMotion);

    lastTime = glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
