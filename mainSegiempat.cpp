#include <gl/glut.h>

void SegiEmpat(void)
{
    // Membersihkan layar sebelum menggambar [cite: 313]
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Menentukan warna segiempat (Merah) [cite: 314]
    glColor3f(1.0f, 0.0f, 0.0f);
    
    // Menggambar segiempat penuh dengan koordinat tertentu [cite: 314, 315]
    glRectf(-0.18, 0.18, 0.18, -0.18);
    
    // Memastikan perintah segera dieksekusi [cite: 316]
    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    
    // Menentukan mode tampilan jendela 
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    
    // Membuat jendela dengan judul "Segi Empat" [cite: 323, 324]
    glutCreateWindow("Segi Empat");
    
    // Memanggil fungsi SegiEmpat untuk ditampilkan [cite: 324]
    glutDisplayFunc(SegiEmpat);
    
    // Latar belakang jendela (Biru) [cite: 327]
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    glutMainLoop();
    return 0;
}
