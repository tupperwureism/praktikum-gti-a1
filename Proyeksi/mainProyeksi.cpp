#include <GL/glut.h>

void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    // Menggambar Poligon (Segitiga 3D)
    glBegin(GL_POLYGON);
        glColor3f(0.0, 0.0, 0.0);       // Warna Hitam
        glVertex3f(-0.5, -0.5, -3.0);   // Titik 1
        
        glColor3f(1.0, 0.0, 0.0);       // Warna Merah
        glVertex3f(0.5, -0.5, -3.0);    // Titik 2
        
        glColor3f(0.0, 0.0, 1.0);       // Warna Biru
        glVertex3f(0.5, 0.5, -3.0);     // Titik 3
    glEnd();
    
    glFlush(); // Selesai rendering
}

void Reshape(int x, int y)
{
    // Jika jendela terminimize (tak ada lagi yang muncul), maka berhenti
    if (y == 0 || x == 0) return; 
    
    // Mengatur proyeksi matrix baru
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    
    /* gluPerspective(fov, aspect, near, far)
       Sudut pandang: 40 derajat
       Jarak potongan dataran terdekat: 0.5
       Jarak potongan dataran terjauh: 20.0 
    */
    gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.5, 20.0);
    
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, x, y); // Gunakan seluruh window untuk rendering
}

int main(int argc, char **argv)
{
    // Inisialisasi GLUT
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Intro");
    
    glClearColor(0.0, 0.0, 0.0, 0.0); // Latar belakang hitam
    
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape); // Mengatur kembali bentuk objek saat window di-resize
    
    glutMainLoop();
    return 0;
}
