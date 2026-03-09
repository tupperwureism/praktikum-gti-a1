#include <GL/glut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    float x_awal = -0.6f;
    float lebar = 0.25f;
    float tinggi_lantai1 = -0.35f;
    float tinggi_lantai2 = -0.1f;

    for (int i = 0; i < 5; i++) {
        float x_pos = x_awal + (i * 0.2f); // Jarak antar kubus
        float y_pos;

        // Logika Ganjil-Genap [cite: 339, 342]
        if (i % 2 == 0) {
            y_pos = tinggi_lantai1; // Tingkat bawah untuk 0, 2, 4
            glColor3f(0.0f, 0.5f, 0.8f); // Biru untuk genap
        } else {
            y_pos = tinggi_lantai2; // Tingkat atas untuk 1, 3
            glColor3f(0.8f, 0.2f, 0.2f); // Merah untuk ganjil
        }

        // Menggambar kubus menggunakan GL_QUADS 
        glBegin(GL_QUADS);
            glVertex2f(x_pos, y_pos);
            glVertex2f(x_pos + lebar, y_pos);
            glVertex2f(x_pos + lebar, y_pos + lebar);
            glVertex2f(x_pos, y_pos + lebar);
        glEnd();
    }

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Tugas Kreatif - 5 Kubus Bertingkat");
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Latar abu gelap biar warna kubus "pop"
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
