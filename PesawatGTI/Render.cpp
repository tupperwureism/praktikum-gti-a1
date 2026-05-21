#include "Game.h"

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

void renderBuilding(int idx){
    Building& b = buildings[idx];
    glPushMatrix();
    glTranslatef(b.x, 0, b.z);

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

void renderShadow(Plane& p) {
    if(!p.alive) return;
    
    glDisable(GL_LIGHTING); 
    
    glPushMatrix();
      glTranslatef(p.pos.x, 0.15f, p.pos.z); 
      glRotatef(p.yaw, 0, 1, 0); 
      glScalef(1.0f, 0.01f, 1.0f); 
      
      glEnable(GL_FOG);
      GLfloat shadowColor[] = {0.1f, 0.1f, 0.1f, 0.8f};
      glFogfv(GL_FOG_COLOR, shadowColor);
      glFogf(GL_FOG_START, 0.0f);
      glFogf(GL_FOG_END,   0.1f);

      glCallList(dlPlane); 
      
      GLfloat skyColor[] = {0.62f, 0.80f, 0.97f, 1.f};
      glFogfv(GL_FOG_COLOR, skyColor);
      glFogf(GL_FOG_START, 80.f);
      glFogf(GL_FOG_END,   350.f);
      
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}

void renderTerrain(){
    float s = MAP_SIZE;
    
    glDisable(GL_LIGHTING); // Matikan pencahayaan agar warna tanah rata (tidak memantulkan cahaya)
    glDisable(GL_FOG);      // Matikan kabut khusus untuk tanah agar warna rumput selalu hijau stabil dan rata (menghilangkan efek "kaca")
    glColor3f(1.0f, 1.0f, 1.0f); // Reset ke warna putih agar warna asli tekstur hijau rumput muncul
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, terrainTex);
    
    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);   glVertex3f(-s, 0, -s);
      glTexCoord2f(s/4.f, 0.0f);  glVertex3f( s, 0, -s);
      glTexCoord2f(s/4.f, s/4.f); glVertex3f( s, 0,  s);
      glTexCoord2f(0.0f, s/4.f);  glVertex3f(-s, 0,  s);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Jalan Aspal
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, roadTex);

    glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);  glVertex3f(-8.f, 0.05f, -s);
      glTexCoord2f(4.0f, 0.0f);  glVertex3f( 8.f, 0.05f, -s);
      glTexCoord2f(4.0f, 50.0f); glVertex3f( 8.f, 0.05f,  s);
      glTexCoord2f(0.0f, 50.0f); glVertex3f(-8.f, 0.05f,  s);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // Garis Putih
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    for(int z = -s; z < s; z += 10) {
        glVertex3f(-0.5f, 0.08f, z);
        glVertex3f( 0.5f, 0.08f, z);
        glVertex3f( 0.5f, 0.08f, z + 5);
        glVertex3f(-0.5f, 0.08f, z + 5);
    }
    glEnd();
    
    glEnable(GL_FOG);      // Aktifkan kembali kabut untuk objek lain
    glEnable(GL_LIGHTING); // Aktifkan kembali pencahayaan untuk objek lain
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

void renderText2D(float x, float y, const char* str, float r, float g, float b){
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

void setupCamera(){
    float yawR   = player.yaw   * PI/180.f;
    float pitchR = player.pitch * PI/180.f;

    Vec3 forward = { sinf(yawR)*cosf(pitchR), -sinf(pitchR), cosf(yawR)*cosf(pitchR) };

    if(camMode == 0){
        Vec3 camPos = player.pos - forward*12.f + vec3(0,4,0);
        Vec3 lookAt = player.pos + forward*3.f;
        gluLookAt(camPos.x, camPos.y, camPos.z, lookAt.x, lookAt.y, lookAt.z, 0,1,0);
    } else {
        float orY = camOrbitYaw   * PI/180.f;
        float orP = camOrbitPitch * PI/180.f;
        float cx_ = player.pos.x + sinf(orY)*cosf(orP)*camOrbitDist;
        float cy_ = player.pos.y + sinf(orP)*camOrbitDist;
        float cz_ = player.pos.z + cosf(orY)*cosf(orP)*camOrbitDist;
        gluLookAt(cx_,cy_,cz_, player.pos.x,player.pos.y,player.pos.z, 0,1,0);
    }
}

bool worldToScreen(Vec3 pos, int W, int H, float& sx, float& sy){
    GLdouble m[16], p[16]; GLint v[4]; GLdouble wx, wy, wz;
    glGetDoublev(GL_MODELVIEW_MATRIX, m); glGetDoublev(GL_PROJECTION_MATRIX, p); glGetIntegerv(GL_VIEWPORT, v);
    if(gluProject(pos.x,pos.y,pos.z, m,p,v, &wx,&wy,&wz) == GL_FALSE || wz < 0 || wz > 1.f) return false;
    sx = (float)wx; sy = (float)wy; return true;
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
