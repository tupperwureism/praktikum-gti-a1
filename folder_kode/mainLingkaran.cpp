#include <gl/glut.h>
#include <math.h>

#define PI 3.1415926535898 // Definisi PI sesuai modul [cite: 107]

void gambarLingkaran() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f); // Warna putih

    glPushMatrix(); // Menggunakan stack [cite: 70]
    glBegin(GL_LINE_LOOP); // Menggunakan primitive LINE_LOOP [cite: 108]
        int circle_points = 100; 
        for (int i = 0; i < circle_points; i++) { 
            float angle = 2 * PI * i / circle_points; 
            glVertex2f(cos(angle) * 0.5, sin(angle) * 0.5); // dikali 0.5 biar ga gede gede amat di layar
        }
    glEnd();
    glPopMatrix();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Tugas 1 - Lingkaran");
    glutDisplayFunc(gambarLingkaran);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // Background biru [cite: 125]
    glutMainLoop();
    return 0;
}
