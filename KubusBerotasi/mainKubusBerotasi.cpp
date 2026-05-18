#include <GL\glut.h>

GLfloat xRotated, yRotated, zRotated;

void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    // Menjauhkan objek dari kamera agar terlihat
    glTranslatef(0.0, 0.0, -4.0);
    
    // Melakukan rotasi pada 3 sumbu (X, Y, Z)
    glRotatef(xRotated, 1.0, 0.0, 0.0);
    glRotatef(yRotated, 0.0, 1.0, 0.0);
    glRotatef(zRotated, 0.0, 0.0, 1.0);
    
    // Mengubah skala (Membuat kubus jadi panjang/balok)
    glScalef(2.0, 1.0, 1.0);
    
    // Menggambar kubus kerangka
    glutWireCube(1.0);
    
    glFlush();
    
    // Menukar buffer (penting untuk animasi halus)
    glutSwapBuffers();
}

void Reshape(int x, int y)
{
    if (y == 0 || x == 0) return;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Menentukan perspektif
    gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 0.5, 20.0);
    
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, x, y);
}

void Idle(void)
{
    // Mengupdate nilai sudut rotasi terus-menerus
    xRotated += 0.3;
    yRotated += 0.1;
    zRotated += -0.4;
    
    // Memanggil ulang fungsi Display
    Display();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    
    // Menggunakan GLUT_DOUBLE untuk animasi yang tidak berkedip
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(300, 300);
    glutCreateWindow("Cube example");
    
    // Mode Wireframe (hanya garis tepi)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    xRotated = yRotated = zRotated = 0.0;
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    
    // Menentukan fungsi yang dipanggil saat program sedang 'diam'
    glutIdleFunc(Idle);
    
    glutMainLoop();
    return 0;
}
