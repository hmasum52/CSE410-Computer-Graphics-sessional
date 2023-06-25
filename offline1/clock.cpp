/**
 * @file clock.cpp
 * @author Hasan Masum (https://hmasum52.github.io/), Dept. of CSE, BUET
 * Level 4, Term 1, CSE 410: Computer Graphics Sessional
 * 
 * @brief 
 * @version 0.1
 * @date 2023-06-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cstdio>

// macro for converting degrees to radians
#define DEG2RAD(x) (x * M_PI / 180.0f)
// RGB to float color conversion
#define RGBTOGL(r, g, b) (r / 255.0f), (g / 255.0f), (b / 255.0f)
//#define CLOCK_COLOR glColor3f(RGBTOGL(111, 196, 208))
#define CLOCK_COLOR glColor3f(RGBTOGL(24, 187, 253))

class Point2D {
public:
    Point2D() : x(0.0), y(0.0) {}
    Point2D(float x, float y) : x(x), y(y) {}

    float x, y;
};

class Color {
public:
    Color() : r(0.0), g(0.0), b(0.0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    float r, g, b;

    // static colors
    static Color red() { return Color(1.0, 0.0, 0.0); }
    static Color green() { return Color(0.0, 1.0, 0.0); }
    static Color blue() { return Color(0.0, 0.0, 1.0); }
    static Color gray() { return Color(0.5, 0.5, 0.5); }
    static Color white() { return Color(1.0, 1.0, 1.0); }
    static Color black() { return Color(0.0, 0.0, 0.0); }
};


const int windowWidth = 800;
const int windowHeight = 800;


// clock parameters
const float clockRadius = 0.4;
const float hourHandLength = 0.08;
const float minuteHandLength = 0.15;
const float secondHandLength = 0.3;

// pendulum parameters
const float pendulumLength = 0.4;
const float pendulumPeriod = 2.0;  // Pendulum period in seconds
const int timerInterval = 10;  // Timer interval in milliseconds

// pendulum physics parameters
// https://tinyurl.com/pendulumeqn
float elapshedTime = 0.0f;          // Current time
const float theta_max = DEG2RAD(15.0f);  // Maximum angle
const float T = 2.0f;  // Time period
const float w = 2.0f * M_PI / T;  // Angular velocity


/* Draw axes: X in Red, Y in Green and Z in Blue */
// for debugging
void drawAxis() {
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

void drawCircle(float centerX, float centerY, float radius) {
    int numSegments = 500;
    float angleStep = 2.0f * M_PI / numSegments;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numSegments; ++i) {
        float angle = i * angleStep;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawFilledCircle(float centerX, float centerY, float radius) {
    int numSegments = 500;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);  // Center of the circle
    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(numSegments);
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawLine(Point2D from, Point2D to, 
            GLfloat width=1.0, Color color = Color::white()) {
    glLineWidth(width);
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_LINES);{
        glVertex2f(from.x, from.y);
        glVertex2f(to.x, to.y);
    }glEnd();
}

void drawMinuteTicks(){
        // draw all 60 ticks
        for (int i = 0; i < 60; ++i) {
            float angle = i * (2.0 * M_PI / 60); // angle in radians
            float x = (clockRadius - 0.06) * cos(angle);
            float y = (clockRadius - 0.06) * sin(angle);

            if(i%5 == 0) {
                // draw the longer tick
                float x2 = (clockRadius - 0.11) * cos(angle);
                float y2 = (clockRadius - 0.11) * sin(angle);
                glLineWidth(3.5);
                glBegin(GL_LINES);{
                    glVertex2d(x, y);
                    glVertex2f(x2, y2);
                }glEnd();
            } else {
                // draw the shorter tick
                float x2 = (clockRadius - 0.09) * cos(angle);
                float y2 = (clockRadius - 0.09) * sin(angle);
                glLineWidth(2.0);
                glBegin(GL_LINES);{
                    glVertex2d(x, y);
                    glVertex2f(x2, y2);
                }glEnd();
            }
        }
}

void drawClockFace() {
    // draw the outside design
    glLineWidth(5.0);
    CLOCK_COLOR;
    glBegin(GL_LINE_LOOP);{
    for (int i = 0; i < 12; ++i) {
        float angle = i * (2.0 * M_PI / 12);
        float x = clockRadius * cos(angle);
        float y = clockRadius * sin(angle);
        glVertex2f(x, y);
    }
    }glEnd();

    // draw the outside circle
    drawCircle(0.0, 0.0, clockRadius-0.05);  // Draw the center of the clock

    // draw the numbers
    glPushMatrix();{
        glColor3f(0.0, 0.0, 0.0);
        glRotated(90, 0, 0, 1);
        glTranslated(-0.01, 0.019, 0.0);  
        //drawAxis();
        // draw the numbers
        for (int i = 1; i <= 12; ++i) {
            float angle = (12-i) * (2.0 * M_PI / 12);
            float shifty;
            if(i > 9) shifty = 0.135;
            else if (i>6) shifty = 0.14;
            else shifty = 0.125;
            float x = (clockRadius - 0.14) * cos(angle);
            float y = (clockRadius - shifty) * sin(angle);
            char str[2];
            sprintf(str, "%d", i);
            glRasterPos2f(x, y);
            for (int j = 0; j < 2; ++j) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[j]);
            }
        }
    }glPopMatrix();

    // draw minute ticks
    glPushMatrix();{
        glRotated(90, 0, 0, 1);
        //glTranslated(-0.015, 0.015, 0.0); // drawAxis();>
        drawMinuteTicks();
    }glPopMatrix();
}

void drawHand(float angle, float length, float scale) {
    glPushMatrix();{
        glBegin(GL_POLYGON);{
            float s = scale;
            float x1 = s * cos(angle + M_PI/2);
            float y1 = s * sin(angle + M_PI/2);

            float x2 = s * cos(angle - M_PI/2);
            float y2 =  s * sin(angle - M_PI/2);

            float t = length;
            float x3 = x2 + t*cos(angle - M_PI/36);
            float y3 = y2 + t*sin(angle - M_PI/36);

            float x4 = x1 + t*cos(angle + M_PI/36);
            float y4 = y1 + t*sin(angle + M_PI/36);

            // color black
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glVertex2f(x3, y3);
            glVertex2f(0.2 * cos(angle), 0.2 * sin(angle));
            glVertex2f(x4, y4);
        };glEnd();
    }glPopMatrix();
}

void drawHourHand(float angle) {
    CLOCK_COLOR;
    drawHand(angle, hourHandLength, 0.005);
}

void drawMinuteHand(float angle) {
    CLOCK_COLOR;
    drawHand(angle, minuteHandLength, 0.005);
}

void drawSecondHand(float angle) {
    glColor3f(0.0, 0.0, 0.0);
    //drawHand(angle, 1.0, secondHandLength);
    glLineWidth(3.0);
    glPushMatrix();{
        //drawAxis();
        glBegin(GL_LINES);
            glVertex2f(0.0, 0.0);
            float x = secondHandLength * cos(angle);
            float y = secondHandLength * sin(angle);
            glVertex2f(x, y);
        glEnd();
    }glPopMatrix();
}


void drawPendulumBox(){
    CLOCK_COLOR;
    for(int i = 8; i<=10 ; i+=2){
        float angle = i * (2.0 * M_PI / 12);
        float x = clockRadius * cos(angle);
        float y = clockRadius * sin(angle);

        glPushMatrix();{
            // draw line 
            glLineWidth(10.0);
            glBegin(GL_LINES);
                glVertex2f(x, y);
                glVertex2f(x, y-0.508);
            glEnd();

            glBegin(GL_LINES);{
                glVertex2f( i == 5? x+0.005 : x-0.005, y-0.5);
                float angle_t =  9* (2.0 * M_PI / 12);
                float xt = clockRadius * cos(angle_t) ;
                float yt = clockRadius * sin(angle_t) ;
                glVertex2f(xt,yt-0.55);
            }glEnd();
        }glPopMatrix();
    }
    // black color
    glColor3f(0.0, 0.0, 0.0);
}

void drawPendulum() {
    drawPendulumBox();

    float angle = 9 * (2.0 * M_PI / 12);
    float start_x = clockRadius * cos(angle);
    float start_y = clockRadius * sin(angle);

    glPushMatrix();{
        // translate to the pivot point
        glTranslatef(start_x, start_y, 0.0);

        // Compute the coordinates of the pendulum bob
        float angle = theta_max * sin(w * elapshedTime); // Current angle
        float bobX = pendulumLength * sin(angle);
        float bobY = -pendulumLength * cos(angle);

        // draw the pivot point
        drawFilledCircle(0.0, 0.0, 0.025);

        glLineWidth(10.0);
        // draw the pendulum rod 
        glBegin(GL_LINES);{
            glVertex2f(0.0f, 0.0f);
            glVertex2f(bobX, bobY);
        }glEnd();

        // draw the pendulum bob
        drawFilledCircle(bobX, bobY, 0.05);
    }glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();              // To operate on Model-View matrix

    // Get the current time
    time_t currentTime = time(nullptr);
    struct tm* localTime = localtime(&currentTime);
    int hour = localTime->tm_hour;
    int minute = localTime->tm_min;
    int second = localTime->tm_sec;

    // Compute the angles for the clock hands
    float hourAngle =  -(hour % 12 + minute / 60.0) * (2.0 * M_PI / 12);
    float minuteAngle = -(minute + second / 60.0) * (2.0 * M_PI / 60);
    float secondAngle = - second * (2.0 * M_PI / 60);


    glPushMatrix();{
        glTranslatef(0, 0.5, 0);

        // Draw the clock face
        drawClockFace();
        drawPendulum();

        // Draw the hour, minute, and second hands
        //glLoadIdentity();
        glRotated(90.0, 0.0, 0.0, 1.0);  // Rotate the clock face to make 12 o'clock at the top
        drawHourHand(hourAngle);
        drawMinuteHand(minuteAngle);
        drawSecondHand(secondAngle);

    }glPopMatrix();

    
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
    elapshedTime += timerInterval/1000.0; // add time in seconds

    glutPostRedisplay();
    glutTimerFunc(timerInterval, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Analog Clock with Pendulum");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(timerInterval, timer, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // white background
    glutMainLoop();
    return 0;
}
