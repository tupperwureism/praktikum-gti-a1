#include <GL/glut.h>
#include <stdlib.h>

static int shoulder = 0, elbow = 0, palm = 0;
static int fBase[5] = {0, 0, 0, 0, 0}; 
static int fTop[5] = {0, 0, 0, 0, 0};

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
}

// [FIX] Menggunakan sumbu Z (0,0,1) agar jari bergerak naik-turun di layar
void drawFullFinger(float x, float y, float z, int baseAngle, int topAngle) {
    glPushMatrix();
        glTranslatef(x, y, z);
        // Ruas Bawah: Rotasi Z untuk gerakan naik-turun
        glRotatef((GLfloat) baseAngle, 0.0, 0.0, 1.0); 
        glTranslatef(0.15, 0.0, 0.0);
        glPushMatrix();
            glScalef(0.3, 0.1, 0.1);
            glutWireCube(1.0);
        glPopMatrix();
        
        // Ruas Atas: Rotasi Z
        glTranslatef(0.15, 0.0, 0.0);
        glRotatef((GLfloat) topAngle, 0.0, 0.0, 1.0); 
        glTranslatef(0.1, 0.0, 0.0);
        glPushMatrix();
            glScalef(0.2, 0.1, 0.1);
            glutWireCube(1.0);
        glPopMatrix();
    glPopMatrix();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    
    // --- LENGAN & SIKU ---
    glTranslatef(-1.0, 0.0, 0.0);
    glRotatef((GLfloat) shoulder, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    glPushMatrix();
        glScalef(2.0, 0.4, 1.0);
        glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat) elbow, 0.0, 0.0, 1.0);
    glTranslatef(1.0, 0.0, 0.0);
    glPushMatrix();
        glScalef(2.0, 0.4, 1.0);
        glutWireCube(1.0);
    glPopMatrix();

    // --- TELAPAK TANGAN ---
    glTranslatef(1.0, 0.0, 0.0);
    glRotatef((GLfloat) palm, 0.0, 0.0, 1.0);
    glTranslatef(0.25, 0.0, 0.0);
    glPushMatrix();
        glScalef(0.5, 0.6, 0.2); 
        glutWireCube(1.0);
    glPopMatrix();

    // --- 5 JARI ---
    drawFullFinger(0.25, 0.25, 0.0, fBase[1], fTop[1]);  // Telunjuk
    drawFullFinger(0.25, 0.10, 0.0, fBase[2], fTop[2]);  // Tengah
    drawFullFinger(0.25, -0.05, 0.0, fBase[3], fTop[3]); // Manis
    drawFullFinger(0.25, -0.20, 0.0, fBase[4], fTop[4]); // Kelingking
    
    glPushMatrix(); // Jempol
        glTranslatef(0.0, 0.3, 0.0);
        glRotatef(45, 0.0, 0.0, 1.0);
        drawFullFinger(0.0, 0.0, 0.0, fBase[0], fTop[0]);
    glPopMatrix();

    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        // --- KONTROL LENGAN ---
        case 's': shoulder = (shoulder + 5) % 360; break;
        case 'S': shoulder = (shoulder - 5) % 360; break;
        case 'e': elbow = (elbow + 5) % 360; break;
        case 'E': elbow = (elbow - 5) % 360; break;
        case 'p': palm = (palm + 5) % 360; break;
        case 'P': palm = (palm - 5) % 360; break;

        // --- TOMBOL RESET (Kembali Normal) ---
        case 'r': 
            shoulder = elbow = palm = 0;
            for(int i=0; i<5; i++) { fBase[i] = 0; fTop[i] = 0; }
            break;

        // --- POSE SPESIAL ---
        case 'm': // Metal Sign
            fBase[1]=0; fTop[1]=0; fBase[2]=90; fTop[2]=90;
            fBase[3]=90; fTop[3]=90; fBase[4]=0; fTop[4]=0; fBase[0]=90;
            break;
        case 'v': // Spiderman Sign
            fBase[1]=0; fTop[1]=0; fBase[2]=90; fTop[2]=90;
            fBase[3]=90; fTop[3]=90; fBase[4]=0; fTop[4]=0; fBase[0]=0;
            break;

        // --- KONTROL PANGKAL JARI (1-5) ---
        case '1': fBase[0] = (fBase[0] + 5) % 120; break;
        case '2': fBase[1] = (fBase[1] + 5) % 120; break;
        case '3': fBase[2] = (fBase[2] + 5) % 120; break;
        case '4': fBase[3] = (fBase[3] + 5) % 120; break;
        case '5': fBase[4] = (fBase[4] + 5) % 120; break;

        // --- KONTROL UJUNG JARI (Q, W, F, T, Y) ---
        // 'e' dan 'r' dihilangkan agar tidak bentrok dengan Siku & Reset
        case 'q': fTop[0] = (fTop[0] + 5) % 120; break;
        case 'w': fTop[1] = (fTop[1] + 5) % 120; break;
        case 'f': fTop[2] = (fTop[2] + 5) % 120; break; // Jari Tengah pakai 'f'
        case 't': fTop[3] = (fTop[3] + 5) % 120; break;
        case 'y': fTop[4] = (fTop[4] + 5) % 120; break;

        case 27: exit(0); break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 600);
    glutCreateWindow("Robot Hand Metal Sign - Chloud");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
