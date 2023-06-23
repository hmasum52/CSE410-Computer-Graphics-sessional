#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>
#include <ctime>

const int windowWidth = 800;
const int windowHeight = 800;
const float clockRadius = 0.4;
const float hourHandLength = 0.2;
const float minuteHandLength = 0.3;
const float secondHandLength = 0.35;
const float pendulumLength = 0.4;
const float pendulumPeriod = 2.0;  // Pendulum period in seconds

float pendulumAngle = 0.0;  // Current pendulum angle

void drawClockFace() {
    glColor3f(0.0, 0.0, 0.0);  // Black color
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 12; ++i) {
        float angle = i * (2.0 * M_PI / 12);
        float x = clockRadius * cos(angle);
        float y = clockRadius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawHourHand(float angle) {
    glColor3f(0.0, 0.0, 1.0);  // Blue color
    glLineWidth(3.0);

    glPushMatrix();
        glBegin(GL_LINES);
            glRotated(90, 0, 0, 1);
            glVertex2f(0.0, 0.0);
            float x = hourHandLength * cos(angle);
            float y = hourHandLength * sin(angle);
            glVertex2f(x, y);
        glEnd();
    glPopMatrix();
}

void drawMinuteHand(float angle) {
    glColor3f(0.0, 1.0, 0.0);  // Green color
    glLineWidth(3.0);
    glPushMatrix();
        glBegin(GL_LINES);
            glRotated(90, 0, 0, 1);
            glVertex2f(0.0, 0.0);
            float x = minuteHandLength * cos(angle);
            float y = minuteHandLength * sin(angle);
            glVertex2f(x, y);
        glEnd();
    glPopMatrix();
}

void drawSecondHand(float angle) {
    glColor3f(1.0, 0.0, 0.0);  // Red color
    glLineWidth(1.5);
    glPushMatrix();
        glBegin(GL_LINES);
            glRotated(90, 0, 0, 1);
            glVertex2f(0.0, 0.0);
            float x = secondHandLength * cos(angle);
            float y = secondHandLength * sin(angle);
            glVertex2f(x, y);
        glEnd();
    glPopMatrix();
}

void drawPendulum() {
    glColor3f(0.5, 0.5, 0.5);  // Gray color
    glLineWidth(2.0);

    
    glBegin(GL_LINES);
    
        glVertex2f(0.0, 0.0);
        float x = pendulumLength * sin(pendulumAngle);
        float y = -pendulumLength * cos(pendulumAngle);
        glVertex2f(x, y);
    glEnd();

}

void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Get the current time
    time_t currentTime = time(nullptr);
    struct tm* localTime = localtime(&currentTime);
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;

    // Compute the angles for the clock hands
    float hourAngle = M_PI/2 - (hour % 12 + minute / 60.0) * (2.0 * M_PI / 12);
    float minuteAngle = M_PI/2 - (minute + second / 60.0) * (2.0 * M_PI / 60);
    float secondAngle = M_PI/2 -  second * (2.0 * M_PI / 60);

    // Draw the clock face
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -1.0);  // Move the clock back in z-axis
    drawClockFace();

    // Draw the hour, minute, and second hands
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -1.0);  // Move the hands back in z-axis
    drawHourHand(hourAngle);
    drawMinuteHand(minuteAngle);
    drawSecondHand(secondAngle);

    // Draw the pendulum
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -1.0);  // Move the pendulum back in z-axis
    drawPendulum();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);

    // Update pendulum angle based on time
    pendulumAngle = (2.0 * M_PI / pendulumPeriod) * fmod(value / 1000.0, pendulumPeriod);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Analog Clock with Pendulum");

    glClearColor(1.0, 1.0, 1.0, 1.0);  // White background color

    glutDisplayFunc(drawScene);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();

    return 0;
}
