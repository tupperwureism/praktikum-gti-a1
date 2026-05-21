#include "Game.h"

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
