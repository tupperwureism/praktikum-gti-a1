#include <gl/glut.h>

void point(void)
{
    glClear(GL_COLOR_BUFFER_BIT); 
    
    // mengatur besar titik menjadi 5.0 [cite: 224, 241]
    glPointSize(5.0f); 
    
    // mulai menggambar titik [cite: 225, 242]
    glBegin(GL_POINTS); 
    
    // warna titik merah (Format RGB: 1, 0, 0) [cite: 226, 243, 244]
    glColor3f(1.0f, 0.0f, 0.0f); 
    
    // posisi titik [cite: 227]
    glVertex3f(0.25, 0.25, 0.0); 
    
    glEnd();
    glFlush(); 
}

int main(int argc, char* argv[]) 
{
    glutInit(&argc, argv); 
    glutInitWindowSize(640, 480); 
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Titik");
    glutDisplayFunc(point); 
    
    // latar belakang jendela [cite: 237]
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glutMainLoop(); 
    return 0; 
}
