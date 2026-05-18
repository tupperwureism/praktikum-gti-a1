#include <stdlib.h> // standard definitions
#include <stdio.h>  // C I/O (for sprintf)
#include <math.h>   // standard definitions
#include <GL/glut.h> // GLUT

// Variabel Global untuk rotasi
double rotAngle = 10;  // rotation angle about y-axis
double rotAngle1 = 10; // rotation angle about x-axis

//------------------------------------------------------------------
// init
// Mengatur nilai default OpenGL
//------------------------------------------------------------------
void init() {
    glClearColor(0, 0, 0, 0); // Warna latar belakang (hitam)
    glClearDepth(1.0);        // Nilai kedalaman latar belakang

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 1000); // Setup proyeksi perspektif

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(      // Setup kamera
        0.0, 0.0, 5.0, // Eye position (posisi mata)
        0.0, 0.0, 0.0, // Lookat position (titik yang dilihat)
        0.0, 1.0, 0.0  // Up direction (arah atas)
    );
}

//------------------------------------------------------------------
// display callback function
// Dipanggil setiap kali aplikasi perlu menggambar ulang dirinya.
//------------------------------------------------------------------
void display() {
    glClear(
        GL_COLOR_BUFFER_BIT | // Bersihkan frame buffer (warna)
        GL_DEPTH_BUFFER_BIT   // Bersihkan depth buffer (kedalaman)
    );

    glPushMatrix(); // Simpan transformasi kamera saat ini
        glRotated(rotAngle, 0, 1, 0);  // Rotasi berdasarkan rotAngle pada sumbu y
        glRotated(rotAngle1, 1, 0, 0); // Rotasi berdasarkan rotAngle1 pada sumbu x
        
        glEnable(GL_COLOR_MATERIAL);   // Spesifikasikan warna objek
        glColor3f(1.0, 0.1, 0.1);      // Warna kemerahan
        glutSolidTeapot(1);            // Gambar teko (teapot)
    glPopMatrix(); // Kembalikan matrix modelview

    glFlush();          // Paksa OpenGL untuk me-render sekarang
    glutSwapBuffers();  // Membuat gambar terlihat (double buffering)
}

//------------------------------------------------------------------
// keyboard callback function
// Dipanggil setiap kali tombol keyboard ditekan.
//------------------------------------------------------------------
void keyboard(unsigned char k, int x, int y) {
    switch (k) {
        case 'a':
            rotAngle += 5; // Tambah rotasi y sebesar 5 derajat
            break;
        case 'l':
            rotAngle -= 5; // Kurangi rotasi y sebesar 5 derajat
            break;
        case 'y':
            rotAngle1 += 5; // Tambah rotasi x sebesar 5 derajat
            break;
        case 'b':
            rotAngle1 -= 5; // Kurangi rotasi x sebesar 5 derajat
            break;
        case 'q':
            exit(0); // Keluar dari aplikasi
            break;
    }
    glutPostRedisplay(); // Perintah untuk menggambar ulang jendela
}

//------------------------------------------------------------------
// main program
// Titik awal eksekusi program
//------------------------------------------------------------------
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(
        GLUT_DOUBLE | // Gunakan double buffering
        GLUT_DEPTH  | // Minta memori untuk z-buffer (depth testing)
        GLUT_RGB      // Set mode warna RGB
    );
    
    glutInitWindowSize(500, 500);         // Mengatur ukuran jendela
    glutCreateWindow("GLUT Example");    // Membuat jendela dengan judul
    
    init(); // Jalankan inisialisasi buatan sendiri
    
    glutDisplayFunc(display);   // Callback untuk menggambar
    glutKeyboardFunc(keyboard); // Callback untuk input keyboard
    
    glutMainLoop(); // Memasuki loop utama GLUT
    return 0;
}
