#include <gl/glut.h>

void Segitiga(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBegin(GL_TRIANGLES);          // Mulai menggambar segitiga
    
    glColor3f(1.0f, 0.0f, 0.0f);    // Warna merah untuk sudut pertama
    glVertex3f(-0.10, -0.10, 0.00);
    
    glColor3f(0.0f, 1.0f, 0.0f);    // Warna hijau untuk sudut kedua
    glVertex3f(0.10, -0.10, 0.00);
    
    glColor3f(0.0f, 0.0f, 1.0f);    // Warna biru untuk sudut ketiga
    glVertex3f(0.00, 0.10, 0.00);
    
    glEnd();
    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Membuat Segitiga");
    
    glutDisplayFunc(Segitiga);
    
    // Latar belakang jendela tetap biru sesuai modul
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    glutMainLoop();
    return 0;
}
