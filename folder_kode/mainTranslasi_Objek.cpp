#include <gl/glut.h>

void SegiEmpat(void)
{
    // Membersihkan layar sebelum menggambar
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Menggeser posisi objek (-0.50 ke kiri, 0.50 ke atas)
    glTranslatef(-0.50, 0.50, 0);
    
    // Menentukan warna segiempat (Merah)
    glColor3f(1.0f, 0.0f, 0.0f);
    
    // Menggambar segiempat dengan koordinat tertentu
    glRectf(-0.18, 0.18, 0.18, -0.18);
    
    // Memastikan seluruh perintah tereksekusi
    glFlush();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    
    // Menentukan mode tampilan jendela
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    
    // Membuat jendela dengan judul "Segi Empat"
    glutCreateWindow("Segi Empat");
    
    // Memanggil fungsi SegiEmpat untuk ditampilkan
    glutDisplayFunc(SegiEmpat);
    
    // Menentukan warna latar belakang jendela (Biru)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    glutMainLoop();
    return 0;
}
