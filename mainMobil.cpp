#include <gl/glut.h>
#include <math.h>

#define PI 3.1415926535898

void drawWheel() {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 30; i++) {
        float angle = 2 * PI * i / 30;
        glVertex2f(cos(angle) * 0.1, sin(angle) * 0.1);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // --- 1. GUNUNG (GL_TRIANGLES) --- 
    glPushMatrix();
        glColor3f(0.0f, 0.5f, 0.0f); // Hijau Tua
        glBegin(GL_TRIANGLES);
            glVertex2f(-1.0, -0.2); glVertex2f(0.0, 0.6); glVertex2f(1.0, -0.2); // Gunung 1
            glColor3f(0.0f, 0.4f, 0.0f); 
            glVertex2f(-0.5, -0.2); glVertex2f(0.5, 0.7); glVertex2f(1.5, -0.2); // Gunung 2
        glEnd();
    glPopMatrix();

    // --- 2. JALAN (GL_QUADS) --- 
    glPushMatrix();
        glColor3f(0.3f, 0.3f, 0.3f); // Abu-abu Aspal
        glRectf(-1.0, -0.8, 1.0, -0.2); 
        
        // Garis Putih Putus-putus
        glColor3f(1.0f, 1.0f, 1.0f);
        for(float x = -0.9; x < 1.0; x += 0.4) {
            glRectf(x, -0.52, x + 0.2, -0.48);
        }
    glPopMatrix();

    // --- 3. POHON (Stack & Loop) --- 
    float treePos[] = {-0.8, -0.4, 0.4, 0.8};
    for(int i=0; i<4; i++) {
        glPushMatrix();
            glTranslatef(treePos[i], -0.2, 0); // Posisi Pohon 
            // Batang
            glColor3f(0.4f, 0.2f, 0.0f);
            glRectf(-0.03, 0.0, 0.03, 0.15);
            // Daun (Segitiga)
            glColor3f(0.0f, 0.8f, 0.0f);
            glBegin(GL_TRIANGLES);
                glVertex2f(-0.1, 0.15); glVertex2f(0.1, 0.15); glVertex2f(0.0, 0.4);
            glEnd();
        glPopMatrix();
    }

    // --- 4. BURUNG (GL_LINE_STRIP) --- 
    glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f);
        glTranslatef(-0.5, 0.7, 0);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-0.05, 0.03); glVertex2f(0.0, 0.0); glVertex2f(0.05, 0.03);
        glEnd();
    glPopMatrix();

    // --- 5. MOBIL (Berdasarkan Kode Sebelumnya) --- [cite: 137, 138]
    glPushMatrix();
        glTranslatef(0.0, -0.4, 0); // Letakkan mobil di atas jalan 
        
        // Badan Mobil
        glColor3f(1.0f, 0.0f, 0.0f);
        glRectf(-0.4, 0.0, 0.4, 0.25); 
        
        // Atap & Jendela
        glColor3f(0.0f, 0.6f, 1.0f); // Biru Kaca
        glBegin(GL_QUADS);
            glVertex2f(-0.25, 0.25); glVertex2f(0.25, 0.25);
            glVertex2f(0.15, 0.4); glVertex2f(-0.15, 0.4);
        glEnd();

        // Roda (Gunakan Stack agar roda tidak menggeser mobil) [cite: 70]
        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix(); glTranslatef(-0.25, 0.0, 0); drawWheel(); glPopMatrix();
        glPushMatrix(); glTranslatef(0.25, 0.0, 0); drawWheel(); glPopMatrix();
    glPopMatrix();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tugas Akhir Praktikum 3 - Mobil & Lingkungan");
    glutDisplayFunc(display);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Langit Biru Cerah 
    glutMainLoop();
    return 0;
}
