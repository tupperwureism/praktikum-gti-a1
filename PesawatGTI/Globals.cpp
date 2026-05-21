#include "Game.h"

Plane     player;
Plane     enemies[ENEMY_COUNT];
Building  buildings[BUILDING_COUNT];
Tree      trees[TREE_COUNT];
Bullet    bullets[BULLET_MAX];
Missile   missiles[MISSILE_MAX];
Particle  particles[PARTICLE_MAX];
Crater    craters[CRATER_MAX];

int   craterCount = 0, score = 0, enemiesKilled = 0, waveCount = 0;
int   scoreFromKills = 0, scoreFromWaves = 0, scoreFromBldg = 0;
float gameTime = 0.f, dt = 0.016f;
int   lastTime = 0;
bool  gameOver = false, gamePaused = false;

int   missileAmmo = 5;
float missileReload = 0.f;

int   challengeLevel = 0;
float challengeTimer = 60.f, enemyBulletDmg = 18.f, enemySpeedMult = 1.0f;
bool  challengePopup = false;
float challengePopupTimer = 0.f;
char  challengeMsg[128] = "";

int   camMode = 0; 
float camOrbitYaw = 0.f, camOrbitPitch= 20.f, camOrbitDist = 18.f;

bool  keyState[256] = {}, specState[256] = {};
int   mouseBtn[3] = {}, lastMouseX, lastMouseY;

GLuint dlBuilding, dlTree, dlPlane, dlPropeller, terrainTex, roadTex;

GLfloat sunDir[4]  = { 0.6f, 1.0f, 0.4f, 0.0f }; 
GLfloat ambLight[4]= { 0.25f,0.25f,0.28f,1.f };
GLfloat difLight[4]= { 0.95f,0.90f,0.80f,1.f };
GLfloat spcLight[4]= { 1.f,  1.f,  0.95f,1.f };

float propAngle = 0.f, lockOnTimer = 0.f;
int   lockedEnemy = -1;
bool  aimAssistOn = true;
