#include <iostream>
#include <GL/glut.h>
using namespace std;

void display(){
    glPointSize(10.0); // 10x10 pixel point
    glBegin(GL_POINTS);
    glVertex2d(0.0, 0.0);
    glVertex2d(0.5, 0);
    glVertex2d(0, 0.5);

    glEnd();

    glBegin(GL_LINES);
    glVertex2d(0.0, 0.0);
    glVertex2d(0.5, 0);
    glVertex2d(0, 0.5);
    glVertex2d(2, 2);
    glEnd();

    glFlush(); // render now
}

int main(int argc, char *argv[]){
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Hello world!");
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
