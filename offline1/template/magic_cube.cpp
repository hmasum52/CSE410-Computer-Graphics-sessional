#define _USE_MATH_DEFINES
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>
using namespace std;

//typedef vector<vector<tuple<double, double, double>>> Matrix;
typedef tuple<double, double, double> Point3D;



class Color {
public:
    GLfloat r, g, b;
    Color() { r= 0; g= 0; b= 0;}
    
    Color(GLfloat _r, GLfloat _g, GLfloat _b) {
        r = _r;
        g = _g;
        b = _b;
    }

    // static colors
    static Color red() { return Color(1, 0, 0); }
    static Color green() { return Color(0, 1, 0); }
    static Color blue() { return Color(0, 0, 1); }
    static Color white() { return Color(1, 1, 1); }
    static Color black() { return Color(0, 0, 0); }
    static Color yellow() { return Color(1, 1, 0); }
    static Color cyan() { return Color(0, 1, 1); }
    static Color magenta() { return Color(1, 0, 1); }
};

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
}

// Global variables
GLfloat eyex = 4, eyey = 4, eyez = 4;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;
bool isAxes = true, isPyramid = true; //false;

// sphere parameters
GLdouble radius = 0.0;

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f(1,0,0);   // Red
        // X axis
        glVertex3f(0,0,0);
        glVertex3f(1,0,0);

        glColor3f(0,1,0);   // Green
        // Y axis
        glVertex3f(0,0,0);
        glVertex3f(0,1,0);

        glColor3f(0,0,1);   // Blue
        // Z axis
        glVertex3f(0,0,0);
        glVertex3f(0,0,1);
    glEnd();
}

/* Draw a Triangle centered at the origin */
void drawTriangle(Color color) {
    glBegin(GL_TRIANGLES);{  // Each set of 3 vertices form a triangle
        glColor3f(color.r, color.g, color.b);  

        glVertex3f( 1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    }glEnd();
}

/* Draw a pyramid centered at the origin */
void drawPyramid() {         // Begin drawing the pyramid with 4 triangles
        Color colors[2] = {Color::cyan(), Color::magenta()};

        GLfloat scale = 1 - sqrt(2) * radius;
        GLfloat height = (radius)/sqrt(3);

        for (int i = 0; i < 4; i++){
            glPushMatrix();{
                glRotatef(90*i, 0, 1, 0); 
                glTranslatef(height, height, height);
                glScalef(scale, scale, scale);
                drawTriangle(colors[i%2]); 
            }glPopMatrix();   
        }
  // Done drawing the pyramid
}

void drawOctaHedron(){
    drawPyramid();

    // mirror pyramid
    glPushMatrix();{
        glRotatef(180, 1, 0, 0);          // Rotate 180 about the (1,0,0)-axis
        drawPyramid();
    }glPopMatrix();
}

/* draw a shphere */
// https://www.songho.ca/opengl/gl_sphere.html
void drawSphereSub(){

    int subdivision = 3;

    // generate vertices for +X face only by intersecting 2 circular planes
    // (longitudinal and latitudinal) at the given longitude/latitude angles
    const float DEG2RAD = acos(-1) / 180.0f;

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    Point3D vertices[pointsPerRow][pointsPerRow];

    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along Y-axis
    float a2;           // latitudinal angle along Z-axis

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for(int i = 0; i < pointsPerRow; ++i){
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for(int j = 0; j < pointsPerRow; ++j){
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
            v[0] *= scale;
            v[1] *= scale;
            v[2] *= scale;

            // add a vertex into array
            vertices[i][j] = make_tuple(v[0], v[1], v[2]); 

        }
    }

    // draw sphere
    glBegin(GL_QUADS);{
        for(int i = 1; i < pointsPerRow; ++i){
            for(int j = 1; j < pointsPerRow; ++j){
                auto [x, y, z] = vertices[i][j];
                glVertex3d(x, y, z);

                tie(x, y, z) = vertices[i][j-1];
                glVertex3d(x, y, z);

                tie(x, y, z) = vertices[i-1][j-1];
                glVertex3d(x, y, z);

                tie(x, y, z) = vertices[i-1][j];
                glVertex3d(x, y, z);
            }
        }
    }glEnd();
}

void drawShphere(){
    
    glColor3f(0, 0, 1);
    glPushMatrix();{
        glScalef(radius, radius, radius);
        drawSphereSub();
    }glPopMatrix();

    glColor3f(0, 1, 0);
    glPushMatrix();{
        glScalef(radius, radius, radius);
        glRotatef(90, 0, 1, 0);          // Rotate 180 about the (0,1,0)-axis
        drawSphereSub();
    }glPopMatrix();

    // blue 
    glColor3f(0, 0, 1);
    glPushMatrix();{
        glScalef(radius, radius, radius);
        glRotatef(180, 0, 1, 0);          // Rotate 180 about the (0,1,0)-axis
        drawSphereSub();
    }glPopMatrix();

    glColor3f(0, 1, 0);
    glPushMatrix();{
        glScalef(radius, radius, radius);
        glRotatef(270, 0, 1, 0);          // Rotate 180 about the (0,1,0)-axis
        drawSphereSub();
    }glPopMatrix();

    glColor3f(1, 0, 0);
    glPushMatrix();{
        glScalef(radius, radius, radius);
        glRotatef(90, 0, 0, 1);          // Rotate 180 about the (0,1,0)-axis
        drawSphereSub();
    }glPopMatrix();

    glColor3f(1, 0, 0);
    glPushMatrix();{
        glScalef(radius, radius, radius);
        glRotatef(270, 0, 0, 1);          // Rotate 180 about the (0,1,0)-axis
        drawSphereSub();
    }glPopMatrix();
}

void drawSphere(){

    GLfloat r = 1-sqrt(2)*radius;

    glPushMatrix();{
        glTranslatef(r, 0, 0);
        drawShphere();
    }glPopMatrix();


    glPushMatrix();{
        glTranslatef(-r, 0, 0);
        drawShphere();
    }glPopMatrix();

    glPushMatrix();{
        glTranslatef(0, r, 0);
        drawShphere();
    }glPopMatrix();

    glPushMatrix();{
        glTranslatef(0, -r, 0);
        drawShphere();
    }glPopMatrix();

    glPushMatrix();{
        glTranslatef(0, 0, r);
        drawShphere();
    }glPopMatrix();


    glPushMatrix();{
        glTranslatef(0, 0, -r);
        drawShphere();
    }glPopMatrix();
}

void drawCylinderSegment(double height, double radius, int segments) {
    double tempx = radius, tempy = 0;
    double currx, curry;
    glBegin(GL_QUADS);
        for (int i = 1; i <= segments; i++) {
            double theta = i * 2.0 * M_PI / segments;
            currx = radius * cos(theta);
            curry = radius * sin(theta);

            GLfloat c = (2+cos(theta))/3;
            glColor3f(1,1,0);
            glVertex3f(currx, curry, height/2);
            glVertex3f(currx, curry, -height/2);

            glVertex3f(tempx, tempy, -height/2);
            glVertex3f(tempx, tempy, height/2);

            tempx = currx;
            tempy = curry;
        }
    glEnd();
}


void drawAllCylinder(){
    GLfloat r = 1-sqrt(2)*radius;   // sphere center along x axis
    GLfloat h = sqrt(2) - 2*radius; // cylinder height

    glColor3f(1, 1, 1);

    // draw 4 cylinder in xz plane
    for(int i = 0; i < 4; ++i){
        glPushMatrix();{
            glRotated(90*i, 0, 1, 0);
            //drawCylinderInXZPlane();
            glTranslated(r/2, 0, r/2);
            glRotated(-45, 0, 1, 0);
            drawCylinderSegment(h, radius, 100);

        }glPopMatrix();
    }

    
    for(int i = 0; i < 4; ++i){
        glPushMatrix();{
            glRotated(90*i, 0, 1, 0);

            glColor3f(1, 0, 0);
            glTranslated(0, r/2, r/2);
            glRotated(45, 1, 0, 0);
            drawCylinderSegment(h, radius, 100);
        }glPopMatrix();
    }


    
    for(int i = 0; i < 4; ++i){
        glPushMatrix();{
            glRotated(90*i, 0, 1, 0);

            glColor3f(1, 0, 0);
            glTranslated(0, -r/2, r/2);
            glRotated(-45, 1, 0, 0);
            drawCylinderSegment(h, radius, 100);
        }glPopMatrix();
    }
   
}



/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display() {
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix

    // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    // for 3D view
    gluLookAt(eyex,eyey,eyez,
            centerx,centery,centerz,
            upx,upy,upz);

    // draw
    ///if (isAxes) drawAxes();
    if (isPyramid) drawAllCylinder();
    if (isPyramid) drawOctaHedron();
    if (isPyramid) drawSphere();


    glutSwapBuffers();  // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    /*if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }*/

    // Enable perspective projection with fovy, aspect, zNear and zFar
    // for 3D view
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}


void rotateObject(bool isAntiClockwise=false){
    double v = 0.25;           
    double lx = centerx - eyex; // look vector x
    double lz = centerz - eyez; // look vector z

    int dir = isAntiClockwise ? 1 : -1;

    eyex += v * (dir*upy*lz);
    eyez += v * (-dir*lx*upy);
    double s = sqrt(eyex*eyex + eyez*eyez) / (4 * sqrt(2));
    eyex /= s;
    eyez /= s;
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y) {
    float v = 0.1;
    switch (key) {
    // Control eye (location of the eye)
    // control eyex
    case '1':
        eyex += v;
        break;
    case '2':
        eyex -= v;
        break;
    // control eyey
    case '3':
        eyey += v;
        break;
    case '4':
        eyey -= v;
        break;
    // control eyez
    case '5':
        eyez += v;
        break;
    case '6':
        eyez -= v;
        break;

    // Control center (location where the eye is looking at)
    // control centerx
    case 'q':
        centerx += v;
        break;
    case 'w':
        centerx -= v;
        break;
    // control centery
    case 'e':
        centery += v;
        break;
    case 'r':
        centery -= v;
        break;
    // control centerz
    case 't':
        centerz += v;
        break;
    case 'y':
        centerz -= v;
        break;

    // Controlling Object
    case 'a':
        rotateObject(); // rotate clockwise
        break;
    case 'd':
        rotateObject(true); // rotate anti-clockwise
        break;
    case '.':
        
        radius += 0.045;
        if(radius>1/sqrt(2))
            radius = 1/sqrt(2);
        break;
    case ',':
        
        radius -= 0.045;
        if(radius<0)
            radius = 0;
        break;

    // Control exit
    case 27:    // ESC key
        exit(0);    // Exit window
        break;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x,int y) {
    double v = 0.25;           
    double lx = centerx - eyex; // look vector x
    double lz = centerz - eyez; // look vector z
    double s;                   // scaling factor
    switch (key) {
    case GLUT_KEY_LEFT: // rotate anti-clockwise
        eyex += v * (upy*lz);
        eyez += v * (-lx*upy);
        s = sqrt(eyex*eyex + eyez*eyez) / (4 * sqrt(2));
        eyex /= s;
        eyez /= s;
        break;
    case GLUT_KEY_RIGHT: // rotate clockwise
        eyex += v * (-upy*lz);
        eyez += v * (lx*upy);
        s = sqrt(eyex*eyex + eyez*eyez) / (4 * sqrt(2));
        eyex /= s;
        eyez /= s;
        break;
    case GLUT_KEY_UP:
        eyey += v;
        break;
    case GLUT_KEY_DOWN:
        eyey -= v;
        break;
    
    default:
        return;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                      // Initialize GLUT
    glutInitWindowSize(640, 640);               // Set the window's initial width & height
    glutInitWindowPosition(50, 50);             // Position the window's initial top-left corner
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");      // Create a window with the given title
    glutDisplayFunc(display);                   // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);           // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);         // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);        // Register callback handler for special-key event
    initGL();                                   // Our own OpenGL initialization
    glutMainLoop();                             // Enter the event-processing loop
    return 0;
}
