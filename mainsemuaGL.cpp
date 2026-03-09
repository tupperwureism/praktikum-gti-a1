#include <GL/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. GL_LINE_STRIP (Garis menyambung tapi tidak menutup)
    glColor3f(1.0f, 1.0f, 0.0f); // Kuning
    glBegin(GL_LINE_STRIP);
        glVertex2f(-0.8f, 0.8f);
        glVertex2f(-0.6f, 0.9f);
        glVertex2f(-0.4f, 0.8f);
        glVertex2f(-0.2f, 0.9f);
    glEnd();

    // 2. GL_LINE_LOOP (Garis menyambung dan menutup jadi poligon)
    glColor3f(0.0f, 1.0f, 0.0f); // Hijau
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.2f, 0.8f);
        glVertex2f(0.4f, 0.9f);
        glVertex2f(0.6f, 0.9f);
        glVertex2f(0.8f, 0.8f);
    glEnd();

    // 3. GL_TRIANGLE_FAN (titik lain menghubungkan dirinya dengan si titik pusat (seperti kipas, ada anchornya))
	glBegin(GL_TRIANGLE_FAN);
	    glColor3f(1.0f, 1.0f, 1.0f); // Pusat Putih
	    glVertex2f(-0.5f, 0.4f); 
	    glColor3f(1.0f, 0.0f, 0.0f); // Merah
	    glVertex2f(-0.7f, 0.2f);
	    glColor3f(0.0f, 1.0f, 0.0f); // Hijau
	    glVertex2f(-0.5f, 0.1f);
	    glColor3f(0.0f, 0.0f, 1.0f); // Biru
	    glVertex2f(-0.3f, 0.2f);
	glEnd();

    // 4. GL_TRIANGLE_STRIP (semua titik saling menghubungkan satu sama lain, seperti soal kombinasi menghubungkan titik titik yang ada)
    glColor3f(0.0f, 1.0f, 1.0f); // Cyan
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0.3f, 0.4f);
        glVertex2f(0.3f, 0.2f);
        glVertex2f(0.5f, 0.4f);
        glVertex2f(0.5f, 0.2f);
    glEnd();

    // 5. GL_QUADS (Segi empat terpisah, jadi vertexnya bisanya kelipatan 4, kalo nggak, vertex sisanya akan diabaikan)
    glColor3f(0.8f, 0.0f, 0.8f); // Ungu
    glBegin(GL_QUADS);
        glVertex2f(-0.7f, -0.3f);
        glVertex2f(-0.3f, -0.3f);
        glVertex2f(-0.3f, -0.6f);
        glVertex2f(-0.7f, -0.6f);
    glEnd();

    // 6. GL_QUAD_STRIP (membuat garis dari titik sekarang (titik n) dengan titik sebelumnya (titik n-1) secara sekuensial)
	glBegin(GL_QUAD_STRIP);
	    glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.2f, -0.3f);
	    glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(0.2f, -0.6f);
	    glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(0.5f, -0.3f);
	    glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(0.5f, -0.6f);
	glEnd();

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tugas Primitif Lengkap - Chloud");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Latar hitam biar kontras
    glutDisplayFunc(display);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //ini lihat kerangka doang
    glutMainLoop();
    return 0;
}
