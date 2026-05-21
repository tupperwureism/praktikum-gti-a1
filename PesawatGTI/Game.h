#ifndef GAME_H
#define GAME_H

#ifdef _WIN32
  #include <windows.h>
#endif

#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>
#include <ctime>

// --- KONSTANTA ---
#define MAP_SIZE        200.0f
#define BUILDING_COUNT   30
#define TREE_COUNT       40
#define ENEMY_COUNT       5
#define BULLET_MAX       30
#define MISSILE_MAX       6
#define PARTICLE_MAX    200
#define CRATER_MAX       50

static const float PI = 3.14159265f;
static const float MISSILE_RELOAD_TIME = 8.f;

// --- STRUKTUR DATA ---
struct Vec3 { float x, y, z; };
inline Vec3 vec3(float x, float y, float z){ return {x,y,z}; }
inline Vec3 operator+(Vec3 a, Vec3 b){ return {a.x+b.x, a.y+b.y, a.z+b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b){ return {a.x-b.x, a.y-b.y, a.z-b.z}; }
inline Vec3 operator*(Vec3 a, float s){ return {a.x*s, a.y*s, a.z*s}; }
inline float vlen(Vec3 v){ return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z); }
inline Vec3 vnorm(Vec3 v){ float l=vlen(v); if(l<0.0001f)return{0,0,0}; return v*(1.f/l); }
inline float vdot(Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}

struct Building { float x, z, w, d, h, r, g, b; bool damaged; int crackLevel; };
struct Tree     { float x, z, height, trunkR; };
struct Plane    { Vec3 pos, vel, aiTarget; float yaw, pitch, roll, health, shootCooldown, aiTimer; bool alive; };
struct Bullet   { Vec3 pos, vel; bool active, fromPlayer; float life; };
struct Missile  { Vec3 pos, vel; bool active; int targetIdx; float life, turnSpeed; };
struct Particle { Vec3 pos, vel; float r, g, b, a, life, size; bool active, isFire; };
struct Crater   { Vec3 pos; float radius, fireTimer; int buildingIdx; bool onFire; };

// --- EXTERN VARIABEL GLOBAL ---
extern Plane     player;
extern Plane     enemies[ENEMY_COUNT];
extern Building  buildings[BUILDING_COUNT];
extern Tree      trees[TREE_COUNT];
extern Bullet    bullets[BULLET_MAX];
extern Missile   missiles[MISSILE_MAX];
extern Particle  particles[PARTICLE_MAX];
extern Crater    craters[CRATER_MAX];

extern int   craterCount, score, enemiesKilled, waveCount;
extern int   scoreFromKills, scoreFromWaves, scoreFromBldg;
extern float gameTime, dt;
extern int   lastTime;
extern bool  gameOver, gamePaused;

extern int   missileAmmo;
extern float missileReload;

extern int   challengeLevel;
extern float challengeTimer, enemyBulletDmg, enemySpeedMult;
extern bool  challengePopup;
extern float challengePopupTimer;
extern char  challengeMsg[128];

extern int   camMode;
extern float camOrbitYaw, camOrbitPitch, camOrbitDist;

extern bool  keyState[256], specState[256];
extern int   mouseBtn[3], lastMouseX, lastMouseY;

extern GLuint dlBuilding, dlTree, dlPlane, dlPropeller, terrainTex, roadTex;

extern GLfloat sunDir[4];
extern GLfloat ambLight[4];
extern GLfloat difLight[4];
extern GLfloat spcLight[4];

extern float propAngle, lockOnTimer;
extern int   lockedEnemy;
extern bool  aimAssistOn;

// --- DEKLARASI FUNGSI ---
// Logic.cpp
float randf(float lo, float hi);
void spawnParticles(Vec3 pos, int n, bool fire);
void addCrater(Vec3 pos, int buildingIdx, bool onFire);
void initBuildings();
void initTrees();
void initPlayer();
void initEnemies();
void initBullets();
void initParticles();
int  findBestLockTarget();
Vec3 calcLeadPoint(int enemyIdx);
void fireBullet(Plane& p, bool fromPlayer);
bool sphereAABB(Vec3 center, float r, float bx,float bz,float bw,float bd,float bh);
void checkBulletBuildingCollision(Bullet& bullet);
void checkBulletPlaneCollision(Bullet& bullet);
void fireMissile();
void updateMissiles(float dt_);
void triggerChallenge();
void updateEnemy(Plane& e, float dt_);
void updatePlayer(float dt_);

// Render.cpp
void setupTextures();
void buildDisplayLists();
void drawBox(float w, float h, float d);
void renderBuilding(int idx);
void renderTree(Tree& t);
void renderPlane(Plane& p, bool isPlayer, bool isEnemy);
void renderShadow(Plane& p);
void renderTerrain();
void renderSky();
void renderBullets();
void renderMissiles();
void renderParticles();
void renderText2D(float x, float y, const char* str, float r=1,float g=1,float b=1);
void renderHUD();
void setupCamera();
bool worldToScreen(Vec3 pos, int W, int H, float& sx, float& sy);
void display();

#endif
