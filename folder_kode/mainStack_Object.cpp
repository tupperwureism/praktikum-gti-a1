#include <gl/glut.h>
#include <gl/gl.h>
#include <math.h>

// Definisi nilai PI untuk perhitungan lingkaran
#define PI 3.1415926535898

void RenderScene(void)
{
    // Membersihkan layar dengan warna latar belakang
    glClear(GL_COLOR_BUFFER_BIT);

    // --- BLOK 1: MENGGAMBAR GARIS ---
    glPushMatrix();
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glColor3f(1.0f, 1.0f, 1.0f); // Warna Putih
            glVertex3f(0.00, 0.20, 0.0);
            glVertex3f(0.00, -0.20, 0.0);
        glEnd();
    glPopMatrix();

    // --- BLOK 2: MENGGAMBAR TITIK DENGAN TRANSLASI ---
    glPushMatrix();
        glPointSize(5.0f);
        glTranslatef(0.35, 0.35, 0.0); // Geser posisi titik
        glBegin(GL_POINTS);
            glColor3f(1.0f, 1.0f, 1.0f); // Warna Putih
            glVertex3f(0.25, 0.25, 0.0);
        glEnd();
    glPopMatrix();

    // --- BLOK 3: MENGGAMBAR TRIANGLE STRIP ---
    glPushMatrix();
        glTranslatef(0.50, 0.50, 0.00); // Geser posisi bidang
        glBegin(GL_TRIANGLE_STRIP);
            glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-0.05, -0.05, 0.00); // Merah
            glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.15, -0.05, 0.00);  // Hijau
            glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(-0.05, 0.05, 0.00);  // Biru
            glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(0.15, 0.05, 0.00);   // Kuning
        glEnd();
    glPopMatrix();

    // --- BLOK 4: MENGGAMBAR LINGKARAN (LINE LOOP) ---
    glPushMatrix();
        glBegin(GL_LINE_LOOP);
            GLint circle_points = 100; // Jumlah titik pembentuk lingkaran, mkn bnyk makin besar
            int i;
            float angle;
            for (i = 0; i < circle_points; i++) {
                angle = 2 * PI * i / circle_points;
                glVertex2f(cos(angle), sin(angle)); // Menghitung koordinat x,y
            }
        glEnd();
    glPopMatrix();

    glFlush(); // Memastikan semua perintah digambar
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Simple"); // Judul jendela sesuai gambar
    
    glutDisplayFunc(RenderScene);
    
    // Latar belakang jendela diset Biru
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    glutMainLoop();
    return 0;
}
