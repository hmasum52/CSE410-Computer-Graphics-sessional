#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>

// macro for converting degrees to radians
#define DEG2RAD(x) (x * M_PI / 180.0f)

const int windowWidth = 800;
const int windowHeight = 600;
const int timerInterval = 10;  // Timer interval in milliseconds

float time = 0.0f;          // Current time
const float theta_max = DEG2RAD(30.0f);  // Maximum angle
const float T = 2.0f;  // Time period
const float w = 2.0f * M_PI / T;  // Angular velocity

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f(1,0,0);   // Red
        // X axis
        glVertex3f(0,0,0);
        glVertex3f(2,0,0);

        glColor3f(0,1,0);   // Green
        // Y axis
        glVertex3f(0,0,0);
        glVertex3f(0,2,0);

        glColor3f(0,0,1);   // Blue
        // Z axis
        glVertex3f(0,0,0);
        glVertex3f(0,0,2);
    glEnd();
}


void drawPendulum() {
    const float rodLength = 200.0f;
    const float bobRadius = 30.0f;

    // Compute the coordinates of the pendulum bob
    float angle = theta_max * sin(w * time); // Current angle
    float bobX = rodLength * sin(angle);
    float bobY = -rodLength * cos(angle);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw the pendulum rod
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(bobX, bobY);
    glEnd();

    // Draw the pendulum bob
    glTranslatef(bobX, bobY, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);  // Red color
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; ++i) {
        float angle = 2.0f * M_PI * static_cast<float>(i) / 360.0f;
        float x = bobRadius * cos(angle);
        float y = bobRadius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    glFlush();
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
}

void displayCallback() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // White background
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-windowWidth / 2, windowWidth / 2, -windowHeight / 2, windowHeight / 2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawPendulum();

    glFlush();
    glutSwapBuffers();      // Swap front and back buffers (of double buffered mode)
}

void update(int value) {
    time += 0.01f;

    glutPostRedisplay();
    glutTimerFunc(timerInterval, update, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL Pendulum Swing");

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshape);
    glutTimerFunc(timerInterval, update, 0);

    glutMainLoop();

    return 0;
}