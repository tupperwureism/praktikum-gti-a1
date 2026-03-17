#include <gl/glut.h>

void Garis(void)
{
    glClear(GL_COLOR_BUFFER_BIT); // Membersihkan layar [cite: 254]
    glLineWidth(2.0f);            // Menentukan lebar garis menjadi 2.0 [cite: 275]
    
    glBegin(GL_LINES);            // Mulai menggambar garis [cite: 256]
    glColor3f(1.0f, 1.0f, 1.0f);  // Warna garis putih (RGB: 1, 1, 1) [cite: 257]
    glVertex3f(0.00, 0.20, 0.0);  // Titik pertama (atas) [cite: 258]
    glVertex3f(0.00, -0.20, 0.0); // Titik kedua (bawah) [cite: 259]
    glEnd();                      // Akhiri penggambaran garis [cite: 260]
    
    glFlush();                    // Memastikan perintah tereksekusi [cite: 260]
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);                         // Ukuran jendela [cite: 265]
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);         // Mode tampilan [cite: 266]
    glutCreateWindow("Membuat Garis");                    // Judul jendela [cite: 267]
    glutDisplayFunc(Garis);                               // Memanggil fungsi Garis [cite: 268]
    
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);                 // Latar belakang biru [cite: 271]
    
    glutMainLoop();
    return 0;
}
