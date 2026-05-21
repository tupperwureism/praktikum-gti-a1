// ============================================================
//  AIRPLANE / FLIGHT SHOOTER  –  OpenGL + GLUT
//  Grafik & Teknik Interaktif
//
//  Compile (Linux):
//    g++ main.cpp -o game -lGL -lGLU -lglut -lm
//  Compile (Windows MinGW):
//    g++ main.cpp -o game.exe -lfreeglut -lopengl32 -lglu32 -lm
// ============================================================

#ifdef _WIN32
  #include <windows.h>
#endif

#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>
#include <ctime>

// ============================================================
//  KONSTANTA & STRUKTUR DATA
// ============================================================
#define MAP_SIZE        200.0f
#define BUILDING_COUNT   30
#define TREE_COUNT       40
#define ENEMY_COUNT       5
#define BULLET_MAX       30
#define MISSILE_MAX       6
#define PARTICLE_MAX    200
#define CRATER_MAX       50

static const float PI = 3.14159265f;

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

// --- Forward Declarations ---
Vec3 calcLeadPoint(int enemyIdx);
bool worldToScreen(Vec3 pos, int W, int H, float& sx, float& sy);
int  findBestLockTarget();
void fireMissile();

// ============================================================
//  STATE GLOBAL
// ============================================================
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
const float MISSILE_RELOAD_TIME = 8.f;

int   challengeLevel = 0;
float challengeTimer = 60.f, enemyBulletDmg = 18.f, enemySpeedMult = 1.0f;
bool  challengePopup = false;
float challengePopupTimer = 0.f;
char  challengeMsg[128] = "";

int   camMode = 0; // 0 = 3rd-person follow, 1 = free orbit
float camOrbitYaw = 0.f, camOrbitPitch= 20.f, camOrbitDist = 18.f;

bool  keyState[256] = {}, specState[256] = {};
int   mouseBtn[3] = {}, lastMouseX, lastMouseY;
bool  mouseCapture = false;

GLuint dlBuilding, dlTree, dlPlane, dlPropeller;
GLuint terrainTex, roadTex;

GLfloat sunDir[4]  = { 0.6f, 1.0f, 0.4f, 0.0f }; // w=0.0f sesuai Modul 6 (Cahaya Matahari Jauh)
GLfloat ambLight[4]= { 0.25f,0.25f,0.28f,1.f };
GLfloat difLight[4]= { 0.95f,0.90f,0.80f,1.f };
GLfloat spcLight[4]= { 1.f,  1.f,  0.95f,1.f };

float propAngle = 0.f, lockOnTimer = 0.f;
int   lockedEnemy = -1;
bool  aimAssistOn = true;

// ============================================================
//  UTILITY & INISIALISASI
// ============================================================
float randf(float lo, float hi){ return lo + (hi-lo)*((float)rand()/RAND_MAX); }

void spawnParticles(Vec3 pos, int n, bool fire){
    for(int i=0;i<PARTICLE_MAX&&n>0;i++){
        if(!particles[i].active){
            particles[i].active = true;
            particles[i].isFire = fire;
            particles[i].pos    = pos;
            particles[i].vel    = vec3(randf(-4,4), randf(1,8), randf(-4,4));
            if(fire){
                particles[i].r = randf(0.8f,1.f); particles[i].g = randf(0.1f,0.5f); particles[i].b = 0.f;
                particles[i].size = randf(0.3f,0.9f); particles[i].life = randf(0.4f,1.2f);
            } else {
                particles[i].r = particles[i].g = particles[i].b = randf(0.5f,0.9f);
                particles[i].size = randf(0.1f,0.4f); particles[i].life = randf(0.3f,0.8f);
            }
            particles[i].a = 1.f;
            n--;
        }
    }
}

void addCrater(Vec3 pos, int buildingIdx, bool onFire){
    if(craterCount >= CRATER_MAX) return;

    craters[craterCount] = {
        pos,
        randf(0.4f, 0.9f),
        onFire ? randf(3.0f, 8.0f) : 0.0f,
        buildingIdx,
        onFire
    };

    craterCount++;
}

void initBuildings(){
    srand(42);
    float cx = 0, cz = 0;
    for(int i=0;i<BUILDING_COUNT;i++){
        float angle = randf(0, 2*PI);
        float dist  = (i < 8) ? randf(15,22) : randf(22, MAP_SIZE*0.45f);
        float bx = cx + dist*cosf(angle);
        float bz = cz + dist*sinf(angle);
        if(bx > -12.0f && bx < 0.0f) bx = -14.0f;
        if(bx >= 0.0f && bx < 12.0f) bx =  14.0f;
        buildings[i].x = bx; buildings[i].z = bz;
        if(dist < 22){
            buildings[i].w = randf(3,7); buildings[i].d = randf(3,7); buildings[i].h = randf(10,30);
        } else {
            buildings[i].w = randf(2,5); buildings[i].d = randf(2,5); buildings[i].h = randf(3,12);
        }
        float base = randf(0.35f,0.65f);
        buildings[i].r = base + randf(-0.05f,0.1f);
        buildings[i].g = base + randf(-0.05f,0.05f);
        buildings[i].b = base + randf(0.0f,0.15f);
        buildings[i].damaged    = false;
        buildings[i].crackLevel = 0;
    }
}

void initTrees(){
    srand(123);
    for(int i=0;i<TREE_COUNT;i++){
        float tx = randf(-MAP_SIZE*0.45f, MAP_SIZE*0.45f);
        if(tx > -10.0f && tx < 0.0f) tx = -11.0f;
        if(tx >= 0.0f && tx < 10.0f) tx =  11.0f;
        trees[i].x = tx; trees[i].z = randf(-MAP_SIZE*0.45f, MAP_SIZE*0.45f);
        trees[i].height = randf(2.5f, 5.f); trees[i].trunkR = 0.18f;
    }
}

void initPlayer(){
    player.pos    = {0, 12, -30}; player.vel    = {0, 0,  0};
    player.yaw    = 0;            player.pitch  = 0;           player.roll   = 0;
    player.health = 100.f;        player.alive  = true;        player.shootCooldown = 0;
}

void initEnemies(){
    srand((unsigned)time(0));
    for(int i=0;i<ENEMY_COUNT;i++){
        enemies[i].pos    = {randf(-60,60), randf(8,25), randf(-60,60)};
        enemies[i].vel    = {0,0,0};
        enemies[i].yaw    = randf(0,360); enemies[i].pitch  = 0; enemies[i].roll = 0;
        enemies[i].health = 50.f;         enemies[i].alive  = true;
        enemies[i].shootCooldown = randf(0,2);
        enemies[i].aiTimer= 0;            enemies[i].aiTarget = player.pos;
    }
}

void initBullets(){
    for(int i=0;i<BULLET_MAX;i++) bullets[i].active=false;
    for(int i=0;i<MISSILE_MAX;i++) missiles[i].active=false;
}

void initParticles(){
    for(int i=0;i<PARTICLE_MAX;i++) particles[i].active=false;
}

// === [ IMPLEMENTASI MODUL 2: GAMBAR PRIMITIF ] ===
// Menggunakan GL_QUADS untuk membentuk balok 3D, materi dari Praktikum 2.
void drawBox(float w, float h, float d){
    float hw=w*0.5f, hd=d*0.5f;
    glBegin(GL_QUADS);
    glNormal3f(0,0,1);  glVertex3f(-hw,0,hd); glVertex3f(hw,0,hd); glVertex3f(hw,h,hd); glVertex3f(-hw,h,hd); // Depan
    glNormal3f(0,0,-1); glVertex3f(hw,0,-hd); glVertex3f(-hw,0,-hd);glVertex3f(-hw,h,-hd);glVertex3f(hw,h,-hd); // Belakang
    glNormal3f(-1,0,0); glVertex3f(-hw,0,-hd);glVertex3f(-hw,0,hd); glVertex3f(-hw,h,hd); glVertex3f(-hw,h,-hd); // Kiri
    glNormal3f(1,0,0);  glVertex3f(hw,0,hd);  glVertex3f(hw,0,-hd); glVertex3f(hw,h,-hd); glVertex3f(hw,h,hd); // Kanan
    glNormal3f(0,1,0);  glVertex3f(-hw,h,hd); glVertex3f(hw,h,hd);  glVertex3f(hw,h,-hd); glVertex3f(-hw,h,-hd); // Atas
    glNormal3f(0,-1,0); glVertex3f(-hw,0,-hd);glVertex3f(hw,0,-hd); glVertex3f(hw,0,hd);  glVertex3f(-hw,0,hd); // Bawah
    glEnd();
}

void buildDisplayLists(){
    dlPlane = glGenLists(1);
    glNewList(dlPlane, GL_COMPILE);
    {
        // === [ IMPLEMENTASI MODUL 6: SIFAT MATERIAL PROPERTIES ] ===
        // Objek Logam (Pesawat) diberikan pantulan tinggi (Specular & Shininess)
        GLfloat matSpec[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat matShine[] = {100.0f}; 
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
        
        // Badan (fuselage)
        glColor3f(0.3f, 0.4f, 0.2f);
        GLfloat mat[] = {0.3f, 0.4f, 0.2f, 1.f}; 
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
        glPushMatrix();
          glScalef(0.6f,0.5f,3.0f); drawBox(1,1,1);
        glPopMatrix();

        // Sayap kiri
        GLfloat wingMat[] = {0.25f, 0.35f, 0.15f, 1.f}; 
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, wingMat);
        glPushMatrix();
          glTranslatef(-1.8f,0,0); glScalef(3.2f,0.1f,1.2f); drawBox(1,1,1);
        glPopMatrix();

        // Sayap kanan
        glPushMatrix();
          glTranslatef(1.8f,0,0); glScalef(3.2f,0.1f,1.2f); drawBox(1,1,1);
        glPopMatrix();

        // Ekor horizontal
        glPushMatrix();
          glTranslatef(0,0,-1.2f); glScalef(1.6f,0.08f,0.7f); drawBox(1,1,1);
        glPopMatrix();

        // Ekor vertikal
        glPushMatrix();
          glTranslatef(0,0.3f,-1.3f); glScalef(0.08f,0.8f,0.7f); drawBox(1,1,1);
        glPopMatrix();

        // Kokpit (kaca)
        GLfloat glass[] = {0.4f,0.6f,1.f,0.7f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glass);
        glPushMatrix();
          glTranslatef(0,0.3f,0.6f); glScalef(0.4f,0.35f,0.6f); drawBox(1,1,1);
        glPopMatrix();
    }
    glEndList();

    dlPropeller = glGenLists(1);
    glNewList(dlPropeller, GL_COMPILE);
    {
        GLfloat pmat[] = {0.2f,0.2f,0.2f,1.f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, pmat);

        for(int i = 0; i < 4; i++){
            glPushMatrix();
                glRotatef(i * 90.0f, 0, 0, 1);
                glTranslatef(0, 0.8f, 0);
                glScalef(0.12f, 1.6f, 0.15f);
                drawBox(1,1,1);
            glPopMatrix();
    }

    // pusat baling-baling
    glPushMatrix();
        glutSolidSphere(0.18f, 12, 8);
    glPopMatrix();
}
glEndList();
}

// === [ IMPLEMENTASI MODUL 3: STACK & TRANSLASI ROTASI ] ===
// Menggunakan PushMatrix dan PopMatrix untuk menggambar setiap gedung tanpa saling mengganggu koordinatnya.
void renderBuilding(int idx){
    Building& b = buildings[idx];
    glPushMatrix();
    glTranslatef(b.x, 0, b.z);

    // === [ IMPLEMENTASI MODUL 6: SIFAT MATERIAL PROPERTIES ] ===
    // Material Beton/Semen: Tidak memiliki pantulan (Specular & Shininess dinolkan)
    GLfloat noSpec[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat noShine[] = {0.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, noShine);

    if(b.crackLevel == 2){
        glColor3f(0.4f + 0.2f*(float)(rand()%10)/10.f, 0.15f, 0.05f);
    } else {
        glColor3f(b.r, b.g, b.b);
    }
    drawBox(b.w, b.h, b.d);

    if(b.h > 5.f){
        glColor3f(0.5f, 0.7f, 1.0f);
        int floors = (int)(b.h / 3.f);
        for(int fl=0;fl<floors;fl++){
            float fy = 0.8f + fl*3.f;
            for(int w=0;w<2;w++){
                float fx = (w==0) ? -b.w*0.25f : b.w*0.25f;
                glPushMatrix();
                  glTranslatef(fx, fy, b.d*0.501f);
                  glScalef(b.w*0.18f, 1.0f, 0.02f);
                  drawBox(1,1,1);
                glPopMatrix();
            }
        }
    }

    if(b.crackLevel >= 1){
        glDisable(GL_LIGHTING);
        glColor3f(0.05f, 0.05f, 0.05f);
        glLineWidth(2.f);
        srand(idx * 777);
        glBegin(GL_LINES);
        for(int c=0;c<4;c++){
            float cx_ = randf(-b.w*0.4f, b.w*0.4f);
            float cz_ = b.d*0.505f;
            float cy_  = randf(b.h*0.2f, b.h*0.9f);
            glVertex3f(cx_, cy_, cz_);
            glVertex3f(cx_+randf(-0.8f,0.8f), cy_+randf(-2.f,2.f), cz_);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}

void renderTree(Tree& t){
    glPushMatrix();
    glTranslatef(t.x, 0, t.z);

    GLfloat noSpec[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat noShine[] = {0.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, noShine);

    glColor3f(0.4f, 0.25f, 0.1f);
    glPushMatrix();
      glScalef(t.trunkR*2, t.height*0.45f, t.trunkR*2);
      drawBox(1,1,1);
    glPopMatrix();

    glColor3f(0.1f, 0.55f, 0.12f);
    for(int i=0;i<3;i++){
        glPushMatrix();
          glTranslatef(0, t.height*0.4f + i*t.height*0.18f, 0);
          glutSolidCone(t.height*0.22f*(1.f-i*0.15f), t.height*0.35f, 8, 4);
        glPopMatrix();
    }
    glPopMatrix();
}

//RENDER PESAWAT PLANE 
void renderPlane(Plane& p, bool isPlayer, bool isEnemy){
    if(!p.alive) return;
    glPushMatrix();
    glTranslatef(p.pos.x, p.pos.y, p.pos.z);
    glRotatef(p.yaw,   0,1,0);
    glRotatef(p.pitch, 1,0,0);
    glRotatef(p.roll,  0,0,1);

    if(isEnemy){
        GLfloat emat[]={0.7f,0.1f,0.1f,1.f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, emat);
    }

    glCallList(dlPlane);

    glPushMatrix();
      glTranslatef(0, 0, 1.55f);
      glRotatef(propAngle * (isEnemy?-1.3f:1.f), 0, 0, 1);
      glCallList(dlPropeller);
    glPopMatrix();

    if(isPlayer){
        GLfloat trail[]={0.9f,0.5f,0.1f,1.f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, trail);
        glPushMatrix();
          glTranslatef(0,0,-1.6f);
          glutSolidSphere(0.12f, 6, 4);
        glPopMatrix();
    }
    glPopMatrix();
}

// === [ IMPLEMENTASI MODUL 8: BAYANGAN DENGAN MATRIKS PROYEKSI ] ===
// Fungsi yang diadaptasi langsung dari Modul 8 (halaman 39-40) 
// untuk menghitung matrik proyeksi bayangan yang realistis.
void glShadowProjection(float * l, float * e, float * n) {
    float d, c;
    float mat[16];
    d = n[0]*l[0] + n[1]*l[1] + n[2]*l[2];
    c = e[0]*n[0] + e[1]*n[1] + e[2]*n[2] - d;
    
    mat[0]  = l[0]*n[0]+c; mat[4]  = n[1]*l[0];   mat[8]  = n[2]*l[0];   mat[12] = -l[0]*c-l[0]*d;
    mat[1]  = n[0]*l[1];   mat[5]  = l[1]*n[1]+c; mat[9]  = n[2]*l[1];   mat[13] = -l[1]*c-l[1]*d;
    mat[2]  = n[0]*l[2];   mat[6]  = n[1]*l[2];   mat[10] = l[2]*n[2]+c; mat[14] = -l[2]*c-l[2]*d;
    mat[3]  = n[0];        mat[7]  = n[1];        mat[11] = n[2];        mat[15] = -d;
    
    glMultMatrixf(mat); // Mengalikan matrix bayangan ke sistem
}

// ============================================================
//  RENDER SHADOW (FLATTENING & FOG TRICK)
// ============================================================
// HAPUS fungsi glShadowProjection, lalu timpa renderShadow dengan ini:

void drawPlaneShadowShape(){
    glPushMatrix();
      glScalef(0.6f,0.5f,3.0f);
      drawBox(1,1,1);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(-1.8f,0,0);
      glScalef(3.2f,0.1f,1.2f);
      drawBox(1,1,1);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(1.8f,0,0);
      glScalef(3.2f,0.1f,1.2f);
      drawBox(1,1,1);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(0,0,-1.2f);
      glScalef(1.6f,0.08f,0.7f);
      drawBox(1,1,1);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(0,0.3f,-1.3f);
      glScalef(0.08f,0.8f,0.7f);
      drawBox(1,1,1);
    glPopMatrix();
}


void renderShadow(Plane& p) {
    if(!p.alive) return;
    
    glDisable(GL_LIGHTING); // Matikan pencahayaan
    
    glPushMatrix();
      // Taruh bayangan di atas tanah (ketinggian Y = 0.15f)
      glTranslatef(p.pos.x, 0.15f, p.pos.z); 
      
      // Rotasi mengikuti arah hadap pesawat
      glRotatef(p.yaw, 0, 1, 0); 
      
      // INI KUNCI UTAMANYA: Pipihkan skala sumbu Y pesawat menjadi 0.01 (Gepeng)
      glScalef(1.0f, 0.01f, 1.0f); 
      
      // TRIK CERDAS: Karena display list (dlPlane) memiliki glColor sendiri, 
      // kita gunakan Fog hitam pekat jarak dekat untuk memaksa warnanya jadi gelap
      glEnable(GL_FOG);
      GLfloat shadowColor[] = {0.1f, 0.1f, 0.1f, 0.8f}; // Hitam keabu-abuan
      glFogfv(GL_FOG_COLOR, shadowColor);
      glFogf(GL_FOG_START, 0.0f);
      glFogf(GL_FOG_END,   0.1f); // Jarak 0.1 langsung kabut pekat maksimal

      // Gambar pesawat yang sudah digepengkan dan dihitamkan
      glCallList(dlPlane); 
      
      // KEMBALIKAN PENGATURAN FOG KE WARNA LANGIT SEMULA
      GLfloat skyColor[] = {0.62f, 0.80f, 0.97f, 1.f};
      glFogfv(GL_FOG_COLOR, skyColor);
      glFogf(GL_FOG_START, 80.f);
      glFogf(GL_FOG_END,   350.f);
      
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}

// === [ IMPLEMENTASI MODUL 7: TEKSTUR ] ===
// Menggunakan texture mapping (glTexCoord2f) pada geometri dasar
void renderTerrain(){
    float s = MAP_SIZE;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, terrainTex);
    GLfloat dirtMat[]  = {0.8f, 0.6f, 0.4f, 1.f}; 
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dirtMat);
    
    glBegin(GL_QUADS);
      glNormal3f(0, 1, 0);
      glTexCoord2f(0.0f, 0.0f);   glVertex3f(-s, 0, -s);
      glTexCoord2f(s/4.f, 0.0f);  glVertex3f( s, 0, -s);
      glTexCoord2f(s/4.f, s/4.f); glVertex3f( s, 0,  s);
      glTexCoord2f(0.0f, s/4.f);  glVertex3f(-s, 0,  s);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Jalan Aspal
    // Jalan Aspal dengan tekstur
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, roadTex);

    GLfloat roadMat[] = {0.35f, 0.35f, 0.35f, 1.f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, roadMat);

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 0.0f);  glVertex3f(-8.f, 0.05f, -s);
    glTexCoord2f(4.0f, 0.0f);  glVertex3f( 8.f, 0.05f, -s);
    glTexCoord2f(4.0f, 50.0f); glVertex3f( 8.f, 0.05f,  s);
    glTexCoord2f(0.0f, 50.0f); glVertex3f(-8.f, 0.05f,  s);
    glEnd();

glDisable(GL_TEXTURE_2D);

    // Garis Putih
    GLfloat lineMat[] = {0.9f, 0.9f, 0.9f, 1.f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lineMat);
    glBegin(GL_QUADS);
    for(int z = -s; z < s; z += 10) {
        glVertex3f(-0.5f, 0.08f, z);
        glVertex3f( 0.5f, 0.08f, z);
        glVertex3f( 0.5f, 0.08f, z + 5);
        glVertex3f(-0.5f, 0.08f, z + 5);
    }
    glEnd();
}

void renderSky(){
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float s = MAP_SIZE*2.f;
    float h = MAP_SIZE;

    glBegin(GL_QUADS);
    glColor3f(0.25f,0.52f,0.95f);
    glVertex3f(-s, h, -s); glVertex3f(s, h, -s);
    glVertex3f(s, h,  s);  glVertex3f(-s, h, s);
    glEnd();

    float horizonColor[3]={0.62f,0.80f,0.97f};
    float skyColor[3]    ={0.25f,0.52f,0.95f};
    float sx[5]={-s, s,  s, -s, -s};
    float sz[5]={-s,-s,  s,  s, -s};
    for(int i=0;i<4;i++){
        glBegin(GL_QUADS);
          glColor3fv(horizonColor); glVertex3f(sx[i],  -2, sz[i]);   glVertex3f(sx[i+1],-2, sz[i+1]);
          glColor3fv(skyColor);     glVertex3f(sx[i+1], h, sz[i+1]); glVertex3f(sx[i],   h, sz[i]);
        glEnd();
    }

    glColor3f(1.f,0.95f,0.6f);
    glPushMatrix();
      glTranslatef(80, 120, -180); glutSolidSphere(12, 16, 12);
    glPopMatrix();

    glColor3f(1.f,1.f,1.f);
    float clouds[5][3]={{60,60,-80},{-50,70,30},{20,80,100},{-80,65,-40},{0,55,-120}};
    for(int i = 0; i < 5; i++){
        glPushMatrix();
          glTranslatef(clouds[i][0], clouds[i][1], clouds[i][2]);
          glutSolidSphere(10,8,6);
          glTranslatef(8,0,0); glutSolidSphere(8,8,6);
          glTranslatef(-16,0,0); glutSolidSphere(7,8,6);
        glPopMatrix();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glColor3f(1,1,1);
}

void renderBullets(){
    glDisable(GL_LIGHTING);
    for(int i=0;i<BULLET_MAX;i++){
        if(!bullets[i].active) continue;
        if(bullets[i].fromPlayer) glColor3f(1.f, 0.95f, 0.0f);
        else                      glColor3f(1.f, 0.1f, 0.0f);
        for(int step = 0; step < 4; step++) {
            glPushMatrix();
              Vec3 tailPos = bullets[i].pos - vnorm(bullets[i].vel) * (step * 1.5f);
              glTranslatef(tailPos.x, tailPos.y, tailPos.z);
              float radius = 0.5f - (step * 0.1f); 
              glutSolidSphere(radius, 8, 6);
            glPopMatrix();
        }
    }
    glEnable(GL_LIGHTING);
}

void renderMissiles(){
    glDisable(GL_LIGHTING);
    for(int i=0;i<MISSILE_MAX;i++){
        if(!missiles[i].active) continue;
        Missile& m = missiles[i];
        glPushMatrix();
          glTranslatef(m.pos.x, m.pos.y, m.pos.z);
          glColor3f(0.95f,0.95f,0.95f);
          glutSolidSphere(0.22f, 6, 4);
          glColor3f(1.f,0.4f,0.0f);
          glBegin(GL_LINES);
            glVertex3f(0,0,0);
            glVertex3f(-vnorm(m.vel).x*1.2f,-vnorm(m.vel).y*1.2f,-vnorm(m.vel).z*1.2f);
          glEnd();
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

void renderParticles(){
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    for(int i=0;i<PARTICLE_MAX;i++){
        if(!particles[i].active) continue;
        Particle& p = particles[i];
        glColor4f(p.r,p.g,p.b,p.a);
        glPushMatrix();
          glTranslatef(p.pos.x, p.pos.y, p.pos.z);
          glutSolidSphere(p.size, 5, 3);
        glPopMatrix();
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void renderText2D(float x, float y, const char* str, float r=1,float g=1,float b=1){
    glColor3f(r,g,b);
    glRasterPos2f(x,y);
    for(int i=0;str[i];i++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
}

void renderHUD(){
    int W = glutGet(GLUT_WINDOW_WIDTH);
    int H = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, W, 0, H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    char buf[128];

    // HP Bar
    float hp = player.health / 100.f;
    glColor4f(0.2f,0,0,0.7f);
    glBegin(GL_QUADS); glVertex2f(10,H-30); glVertex2f(210,H-30); glVertex2f(210,H-10); glVertex2f(10,H-10); glEnd();
    glColor4f(0.1f+0.9f*(1-hp), hp*0.9f, 0.f, 0.85f);
    glBegin(GL_QUADS); glVertex2f(10,H-30); glVertex2f(10+200*hp,H-30); glVertex2f(10+200*hp,H-10); glVertex2f(10,H-10); glEnd();
    sprintf(buf,"HP: %.0f%%", player.health); renderText2D(15, H-25, buf);

    // Score
    sprintf(buf,"SCORE: %d", score); renderText2D(W-160, H-20, buf, 1,1,0);
    sprintf(buf,"KILLS: %d/%d", enemiesKilled, ENEMY_COUNT); renderText2D(W-160, H-40, buf, 1,0.5f,0);
    sprintf(buf,"TIME:  %.1fs", gameTime); renderText2D(W-160, H-60, buf, 0.8f,0.8f,1);

    // Kamera Mode
    const char* camNames[]={"3rd Person Follow", "Free Orbit"};
    sprintf(buf,"CAM: %s [C]", camNames[camMode]); renderText2D(10, H-55, buf, 0.7f,0.9f,1);

    // Alt & Speed
    sprintf(buf,"ALT: %.1fm", player.pos.y); renderText2D(10, H-75, buf, 0.6f,1,0.8f);
    float spd = vlen(player.vel); sprintf(buf,"SPD: %.1fm/s", spd); renderText2D(10, H-95, buf, 0.6f,1,0.8f);

    // Aim Assist & Info
    if(aimAssistOn) renderText2D(10, H-115, "AIM ASSIST: ON  [T]", 0.f,1.f,0.5f);
    else            renderText2D(10, H-115, "AIM ASSIST: OFF [T]", 0.5f,0.5f,0.5f);

    renderText2D(10, 116, "Z/V     : Gas/Rem Pesawat", 0.6f,0.6f,0.6f); 
    renderText2D(10, 98,  "WASD    : Gerak Pesawat",    0.6f,0.6f,0.6f);
    renderText2D(10, 80,  "QE      : Roll Kiri/Kanan",  0.6f,0.6f,0.6f);
    renderText2D(10, 62,  "SPACE/X : Tembak/Rudal",     0.6f,0.6f,0.6f);
    renderText2D(10, 44,  "F       : Lock-On Musuh",    0.6f,0.6f,0.6f);
    renderText2D(10, 26,  "T/C/P   : Aim/Kamera/Pause", 0.6f,0.6f,0.6f);

    // Ammo
    int mby = H-140; renderText2D(10, mby+3, "RUDAL:", 1.f,0.7f,0.f);
    for(int i=0;i<5;i++){
        if(i < missileAmmo) glColor4f(1.f,0.5f,0.f,0.9f); else glColor4f(0.3f,0.3f,0.3f,0.6f);
        glBegin(GL_QUADS); float bx=55+i*18.f; glVertex2f(bx,mby); glVertex2f(bx+14,mby); glVertex2f(bx+14,mby+12); glVertex2f(bx,mby+12); glEnd();
    }
    if(missileAmmo < 5 && missileReload > 0){
        float prog = 1.f - (missileReload / MISSILE_RELOAD_TIME);
        glColor4f(0.f,0.8f,1.f,0.7f);
        glBegin(GL_QUADS); glVertex2f(55,mby-6); glVertex2f(55+90*prog,mby-6); glVertex2f(55+90*prog,mby-2); glVertex2f(55,mby-2); glEnd();
    }

    // Challenge
    sprintf(buf,"LVL TANTANGAN: %d", challengeLevel); renderText2D(W-180, H-80, buf, 1.f,0.5f,1.f);
    sprintf(buf,"Tantangan: %.0fs", challengeTimer); renderText2D(W-180, H-100, buf, 0.8f,0.4f,1.f);

    if(challengePopup){
        float alpha = fminf(1.f, challengePopupTimer);
        glColor4f(0.8f,0.f,0.f, alpha*0.75f);
        glBegin(GL_QUADS); glVertex2f(W/2-220,H/2+55); glVertex2f(W/2+220,H/2+55); glVertex2f(W/2+220,H/2+85); glVertex2f(W/2-220,H/2+85); glEnd();
        glColor4f(1.f,1.f,0.f,alpha);
        glRasterPos2f(W/2 - (float)strlen(challengeMsg)*4.5f, H/2+65);
        for(int i=0;challengeMsg[i];i++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, challengeMsg[i]);
    }

    // Crosshair
    int cx=W/2, cy=H/2;
    bool hasLock = (lockedEnemy >= 0 && enemies[lockedEnemy].alive);
    if(hasLock) glColor4f(1.f,0.2f,0.2f,0.95f); else glColor4f(0.f,1.f,0.3f,0.9f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
      glVertex2i(cx-18,cy); glVertex2i(cx-6,cy); glVertex2i(cx+6,cy);  glVertex2i(cx+18,cy);
      glVertex2i(cx,cy-18); glVertex2i(cx,cy-6); glVertex2i(cx,cy+6);  glVertex2i(cx,cy+18);
    glEnd();
    glBegin(GL_LINE_LOOP); for(int a=0;a<16;a++){ float ang=a*PI/8; glVertex2f(cx+cosf(ang)*5, cy+sinf(ang)*5); } glEnd();

    // Lock Indicator
    if(hasLock){
        float ex, ey;
        if(worldToScreen(enemies[lockedEnemy].pos, W, H, ex, ey)){
            float dist3d = vlen(enemies[lockedEnemy].pos - player.pos);
            float rs = fmaxf(14.f, 60.f - dist3d*0.3f);
            glColor4f(1.f,0.2f,0.2f,0.9f); glLineWidth(2.f); float gap = rs*0.35f;
            glBegin(GL_LINES);
              glVertex2f(ex-rs,ey-rs+gap); glVertex2f(ex-rs,ey-rs); glVertex2f(ex-rs,ey-rs); glVertex2f(ex-rs+gap,ey-rs);
              glVertex2f(ex+rs-gap,ey-rs); glVertex2f(ex+rs,ey-rs); glVertex2f(ex+rs,ey-rs); glVertex2f(ex+rs,ey-rs+gap);
              glVertex2f(ex-rs,ey+rs-gap); glVertex2f(ex-rs,ey+rs); glVertex2f(ex-rs,ey+rs); glVertex2f(ex-rs+gap,ey+rs);
              glVertex2f(ex+rs-gap,ey+rs); glVertex2f(ex+rs,ey+rs); glVertex2f(ex+rs,ey+rs); glVertex2f(ex+rs,ey+rs-gap);
            glEnd();
            sprintf(buf,"%.0fm", dist3d); renderText2D(ex+rs+4, ey-4, buf, 1.f,0.4f,0.4f);
            float ehp = enemies[lockedEnemy].health / 50.f;
            glColor4f(0.2f,0.f,0.f,0.7f);
            glBegin(GL_QUADS); glVertex2f(ex-rs,ey+rs+6); glVertex2f(ex+rs,ey+rs+6); glVertex2f(ex+rs,ey+rs+14); glVertex2f(ex-rs,ey+rs+14); glEnd();
            glColor4f(1.f,0.15f,0.15f,0.9f);
            glBegin(GL_QUADS); glVertex2f(ex-rs,ey+rs+6); glVertex2f(ex-rs+rs*2*ehp,ey+rs+6); glVertex2f(ex-rs+rs*2*ehp,ey+rs+14); glVertex2f(ex-rs,ey+rs+14); glEnd();
        }
    }

    if(gamePaused){
        glColor4f(0,0,0,0.55f); glBegin(GL_QUADS); glVertex2i(0,0); glVertex2i(W,0); glVertex2i(W,H); glVertex2i(0,H); glEnd();
        renderText2D(W/2-50,H/2+10,"[PAUSED]",1,1,0); renderText2D(W/2-60,H/2-10,"Tekan P untuk lanjut",0.8f,0.8f,0.8f);
    }
    if(gameOver){
        glColor4f(0,0,0,0.75f); glBegin(GL_QUADS); glVertex2i(0,0); glVertex2i(W,0); glVertex2i(W,H); glVertex2i(0,H); glEnd();
        int px=W/2-200, py=H/2-130;
        glColor4f(0.08f,0.05f,0.12f,0.95f); glBegin(GL_QUADS); glVertex2i(px,py); glVertex2i(px+400,py); glVertex2i(px+400,py+260); glVertex2i(px,py+260); glEnd();
        glColor4f(0.6f,0.1f,0.1f,1.f); glBegin(GL_LINE_LOOP); glVertex2i(px,py); glVertex2i(px+400,py); glVertex2i(px+400,py+260); glVertex2i(px,py+260); glEnd();
        renderText2D(W/2-55, py+230, "=== GAME OVER ===", 1.f,0.15f,0.15f);
        sprintf(buf,"Waktu Bertahan : %02d:%02d", (int)(gameTime/60), (int)gameTime%60); renderText2D(px+20, py+195, buf, 1.f,1.f,0.4f);
        renderText2D(px+20, py+168, "--- Rincian Skor ---", 0.7f,0.7f,1.f);
        sprintf(buf,"Kill Musuh     : +%d  (%d kills)", scoreFromKills, enemiesKilled); renderText2D(px+20, py+148, buf, 0.9f,0.9f,0.9f);
        sprintf(buf,"Selesai Wave   : +%d  (%d wave)", scoreFromWaves, waveCount); renderText2D(px+20, py+128, buf, 0.9f,0.9f,0.9f);
        sprintf(buf,"TOTAL SKOR     : %d", score); renderText2D(px+20, py+75, buf, 1.f,0.85f,0.f);
        renderText2D(px+90, py+8, "Tekan R untuk restart", 0.6f,0.6f,0.6f);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

// === [ IMPLEMENTASI MODUL 5: KAMERA ] ===
// Memanfaatkan gluLookAt untuk mensimulasikan kamera yang bergerak secara independen
void setupCamera(){
    float yawR   = player.yaw   * PI/180.f;
    float pitchR = player.pitch * PI/180.f;

    Vec3 forward = { sinf(yawR)*cosf(pitchR), -sinf(pitchR), cosf(yawR)*cosf(pitchR) };

    if(camMode == 0){
        // 3rd Person Follow
        Vec3 camPos = player.pos - forward*12.f + vec3(0,4,0);
        Vec3 lookAt = player.pos + forward*3.f;
        gluLookAt(camPos.x, camPos.y, camPos.z, lookAt.x, lookAt.y, lookAt.z, 0,1,0);
    } else {
        // Free Orbit
        float orY = camOrbitYaw   * PI/180.f;
        float orP = camOrbitPitch * PI/180.f;
        float cx_ = player.pos.x + sinf(orY)*cosf(orP)*camOrbitDist;
        float cy_ = player.pos.y + sinf(orP)*camOrbitDist;
        float cz_ = player.pos.z + cosf(orY)*cosf(orP)*camOrbitDist;
        gluLookAt(cx_,cy_,cz_, player.pos.x,player.pos.y,player.pos.z, 0,1,0);
    }
}

int findBestLockTarget(){
    float yawR   = player.yaw   * PI/180.f;
    float pitchR = player.pitch * PI/180.f;
    Vec3 fwd = { sinf(yawR)*cosf(pitchR), -sinf(pitchR), cosf(yawR)*cosf(pitchR) };
    int   bestIdx  = -1;
    float bestScore= 0.f;
    for(int i=0;i<ENEMY_COUNT;i++){
        if(!enemies[i].alive) continue;
        Vec3  diff = enemies[i].pos - player.pos;
        float dist = vlen(diff);
        if(dist > 120.f) continue;
        float dot  = vdot(vnorm(diff), fwd);
        if(dot < 0.5f) continue;
        float score = dot / (dist + 1.f);
        if(score > bestScore){ bestScore=score; bestIdx=i; }
    }
    return bestIdx;
}

Vec3 calcLeadPoint(int enemyIdx){
    if(enemyIdx < 0 || !enemies[enemyIdx].alive) return {0,0,0};
    float tof = vlen(enemies[enemyIdx].pos - player.pos) / 65.f;
    return enemies[enemyIdx].pos + enemies[enemyIdx].vel * tof;
}

bool worldToScreen(Vec3 pos, int W, int H, float& sx, float& sy){
    GLdouble m[16], p[16]; GLint v[4]; GLdouble wx, wy, wz;
    glGetDoublev(GL_MODELVIEW_MATRIX, m); glGetDoublev(GL_PROJECTION_MATRIX, p); glGetIntegerv(GL_VIEWPORT, v);
    if(gluProject(pos.x,pos.y,pos.z, m,p,v, &wx,&wy,&wz) == GL_FALSE || wz < 0 || wz > 1.f) return false;
    sx = (float)wx; sy = (float)wy; return true;
}

void fireBullet(Plane& p, bool fromPlayer){
    float yawR   = p.yaw   * PI/180.f;
    float pitchR = p.pitch * PI/180.f;
    Vec3 fwd = { sinf(yawR)*cosf(pitchR), -sinf(pitchR), cosf(yawR)*cosf(pitchR) };
    if(fromPlayer && aimAssistOn && lockedEnemy >= 0 && enemies[lockedEnemy].alive){
        Vec3 toTarget = vnorm(calcLeadPoint(lockedEnemy) - p.pos);
        if(vdot(fwd, toTarget) > 0.6f) fwd = vnorm(fwd * 0.6f + toTarget * 0.4f);
    }
    for(int i=0;i<BULLET_MAX;i++){
        if(!bullets[i].active){
            bullets[i].active = true; bullets[i].fromPlayer = fromPlayer;
            bullets[i].pos = p.pos + fwd*4.5f; bullets[i].vel = fwd * 65.f; bullets[i].life = 3.5f;
            spawnParticles(bullets[i].pos, 15, true); 
            break;
        }
    }
}

bool sphereAABB(Vec3 center, float r, float bx,float bz,float bw,float bd,float bh){
    float dx = fmaxf(bx-r-bw*0.5f, fminf(center.x, bx+bw*0.5f)) - center.x;
    float dy = fmaxf(0-r,          fminf(center.y, bh))          - center.y;
    float dz = fmaxf(bz-r-bd*0.5f, fminf(center.z, bz+bd*0.5f)) - center.z;
    return (dx*dx + dy*dy + dz*dz) < r*r;
}

void checkBulletBuildingCollision(Bullet& bullet){
    if(!bullet.active) return;
    for(int i=0;i<BUILDING_COUNT;i++){
        Building& b = buildings[i];
        if(sphereAABB(bullet.pos, 0.3f, b.x,b.z, b.w,b.d,b.h)){
            bullet.active = false;
            if(b.crackLevel < 2) b.crackLevel++;
            b.damaged = true;
            bool onFire = (b.crackLevel == 2);
            addCrater(bullet.pos, i, onFire);
            spawnParticles(bullet.pos, onFire?20:10, onFire);
            if(bullet.fromPlayer) { scoreFromBldg += 5; score = scoreFromKills+scoreFromWaves+scoreFromBldg; }
            break;
        }
    }
}

void checkBulletPlaneCollision(Bullet& bullet){
    if(!bullet.active) return;
    if(!bullet.fromPlayer && player.alive){
        if(vlen(bullet.pos - player.pos) < 1.8f){
            bullet.active = false; player.health -= enemyBulletDmg; spawnParticles(bullet.pos, 8, false);
            if(player.health <= 0){ player.alive = false; gameOver = true; spawnParticles(player.pos, 60, true); }
        }
    }
    if(bullet.fromPlayer){
        for(int i=0;i<ENEMY_COUNT;i++){
            if(!enemies[i].alive) continue;
            if(vlen(bullet.pos - enemies[i].pos) < 2.f){
                bullet.active = false; enemies[i].health -= 25.f;
                spawnParticles(bullet.pos, 12, enemies[i].health <= 0);
                if(enemies[i].health <= 0){
                    enemies[i].alive = false; enemiesKilled++;
                    scoreFromKills += 100 + (int)(gameTime / 60.f) * 20;
                    score = scoreFromKills + scoreFromWaves + scoreFromBldg;
                    spawnParticles(enemies[i].pos, 50, true);
                }
                break;
            }
        }
    }
}

void fireMissile(){
    if(missileAmmo <= 0 || lockedEnemy < 0 || !enemies[lockedEnemy].alive) return;
    for(int i=0;i<MISSILE_MAX;i++){
        if(!missiles[i].active){
            float yr=player.yaw*PI/180.f, pr=player.pitch*PI/180.f;
            Vec3 fwd={sinf(yr)*cosf(pr),-sinf(pr),cosf(yr)*cosf(pr)};
            missiles[i].active = true; missiles[i].pos = player.pos + fwd*2.5f; missiles[i].vel = fwd * 30.f;
            missiles[i].targetIdx = lockedEnemy; missiles[i].life = 6.f; missiles[i].turnSpeed = 3.5f;
            missileAmmo--;
            if(missileReload <= 0) missileReload = MISSILE_RELOAD_TIME;
            break;
        }
    }
}

void updateMissiles(float dt_){
    for(int i=0;i<MISSILE_MAX;i++){
        if(!missiles[i].active) continue;
        Missile& m = missiles[i];
        m.life -= dt_;
        if(m.life <= 0){ m.active=false; spawnParticles(m.pos,15,true); continue; }
        int ti = m.targetIdx;
        if(ti >= 0 && ti < ENEMY_COUNT && enemies[ti].alive){
            Vec3 newDir = vnorm(vnorm(m.vel) + vnorm(enemies[ti].pos - m.pos) * m.turnSpeed * dt_);
            float spd = vlen(m.vel); if(spd < 50.f) spd += 20.f*dt_;
            m.vel = newDir * spd;
        }
        m.pos = m.pos + m.vel * dt_;
        if(ti >= 0 && ti < ENEMY_COUNT && enemies[ti].alive){
            if(vlen(m.pos - enemies[ti].pos) < 2.5f){
                m.active = false; enemies[ti].health -= 80.f; spawnParticles(m.pos, 40, true);
                if(enemies[ti].health <= 0){
                    enemies[ti].alive = false; enemiesKilled++;
                    scoreFromKills += 150 + (int)(gameTime/60.f)*20;
                    score = scoreFromKills + scoreFromWaves + scoreFromBldg;
                    spawnParticles(enemies[ti].pos, 60, true);
                }
            }
        }
        if(m.pos.y < 0.5f){ m.active=false; spawnParticles(m.pos,20,true); }
    }
}

void triggerChallenge(){
    challengeLevel++; challengePopup = true; challengePopupTimer = 4.f;
    switch(challengeLevel){
        case 1: enemyBulletDmg = 25.f; sprintf(challengeMsg, "MENIT 1: Damage peluru musuh +40%%!"); break;
        case 2: enemySpeedMult = 1.4f; sprintf(challengeMsg, "MENIT 2: Musuh lebih cepat +40%%!"); break;
        case 3: enemyBulletDmg = 32.f; sprintf(challengeMsg, "MENIT 3: Musuh tembak lebih cepat!"); break;
        case 4: enemySpeedMult = 1.8f; enemyBulletDmg = 38.f; sprintf(challengeMsg, "MENIT 4: RAMPAGE MODE!"); break;
        default: enemyBulletDmg += 8.f; enemySpeedMult += 0.2f; sprintf(challengeMsg, "MENIT %d: Musuh makin kuat!", challengeLevel); break;
    }
}

void updateEnemy(Plane& e, float dt_) {
    if (!e.alive) return;
    float margin = MAP_SIZE * 0.8f;
    bool outOfBounds = (abs(e.pos.x) > margin || abs(e.pos.z) > margin);
    if (outOfBounds) e.aiTarget = vec3(0, 15, 0); else e.aiTarget = player.pos;

    Vec3 diff = e.aiTarget - e.pos;
    float dist = vlen(diff);
    Vec3 dir = vnorm(diff);

    float targetYaw = atan2f(dir.x, dir.z) * 180.f / PI;
    float targetPitch = -asinf(dir.y) * 180.f / PI;

    float dyaw = targetYaw - e.yaw;
    while (dyaw > 180) dyaw -= 360; while (dyaw < -180) dyaw += 360;
    e.yaw += dyaw * dt_ * 2.0f;
    e.pitch += (targetPitch - e.pitch) * dt_ * 1.5f;

    float currentSpeed = 18.f * enemySpeedMult;
    if (dist < 15.0f && !outOfBounds) currentSpeed *= 0.5f; 

    float yr = e.yaw * PI / 180.f, pr = e.pitch * PI / 180.f;
    e.vel = vec3(sinf(yr) * cosf(pr), -sinf(pr), cosf(yr) * cosf(pr)) * currentSpeed;
    e.pos = e.pos + e.vel * dt_;

    if (e.pos.y < 5.f) e.pos.y = 5.f;
    if (e.pos.y > 100.f) e.pos.y = 100.f;

    e.shootCooldown -= dt_;
    if (e.shootCooldown <= 0 && dist < 120.f && vdot(vnorm(e.vel), vnorm(player.pos - e.pos)) > 0.5f) {
        fireBullet(e, false);
        float shootInterval = fmaxf(0.5f, 2.5f - challengeLevel * 0.3f);
        e.shootCooldown = randf(shootInterval * 0.7f, shootInterval);
    }
}

void updatePlayer(float dt_){
    if(!player.alive) return;
    float speed = 22.f; 
    if(keyState['z'] || keyState['Z']) speed = 45.f;
    if(keyState['v'] || keyState['V']) speed = 10.f;

    float turnSpd = 80.f, pitchSpd= 60.f, rollSpd = 90.f;
    if(keyState['a']||keyState['A']) player.yaw   -= turnSpd*dt_;
    if(keyState['d']||keyState['D']) player.yaw   += turnSpd*dt_;
    if(keyState['w']||keyState['W']) player.pitch -= pitchSpd*dt_;
    if(keyState['s']||keyState['S']) player.pitch += pitchSpd*dt_;
    if(keyState['q']||keyState['Q']) player.roll  -= rollSpd*dt_;
    if(keyState['e']||keyState['E']) player.roll  += rollSpd*dt_;

    if(player.pitch > 70)  player.pitch = 70;
    if(player.pitch < -70) player.pitch=-70;

    float yr=player.yaw*PI/180.f, pr=player.pitch*PI/180.f;
    Vec3 fwd = { sinf(yr)*cosf(pr), -sinf(pr), cosf(yr)*cosf(pr) };
    player.vel = fwd * speed; player.pos = player.pos + player.vel*dt_;

    float M = MAP_SIZE*0.9f;
    if(player.pos.x<-M) player.pos.x=-M; if(player.pos.x> M) player.pos.x= M;
    if(player.pos.z<-M) player.pos.z=-M; if(player.pos.z> M) player.pos.z= M;
    if(player.pos.y < 2.f){ player.pos.y=2.f; player.pitch=0; }
    if(player.pos.y > 150.f) player.pos.y=150.f;

    player.shootCooldown -= dt_;
    if(keyState[' '] && player.shootCooldown<=0){
        fireBullet(player, true); player.shootCooldown = 0.18f;
    }
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

// === [ IMPLEMENTASI MODUL 4: PROYEKSI & VIEWPORT ] ===
// Memproyeksikan layar 3D menjadi koordinat layar 2D
void reshape(int w, int h){
    if(h==0) h=1;
    glViewport(0,0,w,h);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int W = glutGet(GLUT_WINDOW_WIDTH); int H = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)W/H, 0.3, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    setupCamera();

    glLightfv(GL_LIGHT0, GL_POSITION, sunDir);

    GLfloat fogColor[] = {0.62f,0.80f,0.97f,1.f};
    glFogfv(GL_FOG_COLOR, fogColor); glFogf(GL_FOG_START, 80.f); glFogf(GL_FOG_END,   350.f);

    renderSky();
    renderTerrain();

    for(int i=0;i<BUILDING_COUNT;i++) renderBuilding(i);
    for(int i=0;i<TREE_COUNT;i++)     renderTree(trees[i]);

    renderShadow(player);
    for(int i=0;i<ENEMY_COUNT;i++) renderShadow(enemies[i]);

    renderPlane(player, true, false);
    for(int i=0;i<ENEMY_COUNT;i++) renderPlane(enemies[i], false, true);

    renderBullets();
    renderMissiles();
    renderParticles();
    renderHUD();

    glutSwapBuffers();
}

void keyDown(unsigned char k, int /*x*/, int /*y*/){
    keyState[(unsigned char)k] = true;
    if(k==27){ exit(0); }
    if(k=='p'||k=='P'){ gamePaused=!gamePaused; }
    if(k=='c'||k=='C'){ camMode=(camMode+1)%2; } // Sisa 2 Camera saja
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

void setupTextures() {
    const int texSize = 64;

    unsigned char terrainData[texSize][texSize][3];
    unsigned char roadData[texSize][texSize][3];

    for(int i = 0; i < texSize; i++) {
        for(int j = 0; j < texSize; j++) {
            int noise = rand() % 40;

            terrainData[i][j][0] = 80 + noise;
            terrainData[i][j][1] = 120 + noise;
            terrainData[i][j][2] = 45 + noise / 2;

            int asphalt = 45 + rand() % 25;
            roadData[i][j][0] = asphalt;
            roadData[i][j][1] = asphalt;
            roadData[i][j][2] = asphalt;
        }
    }

    glGenTextures(1, &terrainTex);
    glBindTexture(GL_TEXTURE_2D, terrainTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, terrainData);

    glGenTextures(1, &roadTex);
    glBindTexture(GL_TEXTURE_2D, roadTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, roadData);
}

// === [ IMPLEMENTASI MODUL 6: DEPTH & LIGHTING SETUP ] ===
// Mengaktifkan sistem pencahayaan dan penghalang kedalaman
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
    glutCreateWindow("Airplane City Shooter – OpenGL GLUT");

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