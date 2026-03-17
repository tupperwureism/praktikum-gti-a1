#include <gl/glut.h>

void Segitiga(void)
{
	//wajib dibersihin dulu; 
	//gl clear --> nghapus bekas tulisan spidol di papan tulis
	glClear(GL_COLOR_BUFFER_BIT);
    // Menggeser posisi (0.25 ke kanan, -0.25 ke bawah)
    glTranslatef(0.25, -0.25, 0);
    
    // Memutar objek sebesar 60 derajat terhadap sumbu Z
    glRotated(60.0, 0.0, 0.0, 1.0);
    
    glBegin(GL_TRIANGLES);
        // Titik 1: Merah
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex3f(-0.05, -0.05, 0.00);
        
        // Titik 2: Hijau
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex3f(0.15, -0.05, 0.00);
        
        // Titik 3: Biru
        glColor3f(0.0f, 0.0f, 1.0f); 
        glVertex3f(-0.05, 0.05, 0.00);
    glEnd();
    
    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow("Segiempat");
    
    glutDisplayFunc(Segitiga);
    
    // Warna latar belakang biru tetap dipertahankan
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    glutMainLoop();
    return 0;
}
