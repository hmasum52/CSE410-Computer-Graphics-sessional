/**
 * @file magic_cube.cpp
 * @author Hasan Masum (https://hmasum52.github.io/), Dept. of CSE, BUET
 * Level 4, Term 1, CSE 410: Computer Graphics Sessional
 * online 1
 * 
 * @brief 
 * @version 0.1
 * @date 2023-06-25
 * 
 * 
 */
#define _USE_MATH_DEFINES
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>
using namespace std;

struct Point3D{
    GLfloat x, y, z;
};

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
const GLfloat DEFAULT_ZOOM = 5;


// camera parameters
Point3D eye;        // camera position
Point3D look;       // camera looks at
Point3D r;          // right vector for camera
Point3D up;         // up direction for camera

// sphere parameters
GLdouble radius = 0.0;

// object rotation
GLfloat objAngle = 0.0f;

// camera movement
bool isCameraMoving = false;

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

        //radius = length/sqrt(2);
        GLfloat scale = 1 - sqrt(2) * radius;
        GLfloat t = (radius)/sqrt(3);

        for (int i = 0; i < 4; i++){
            glPushMatrix();{
                glRotatef(90*i, 0, 1, 0); 
                glTranslatef(t, t, t);
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
void drawCubesphere(Color color){
    int subdivision = 3;

    // generate vertices for +X face only by intersecting 2 circular planes
    // (longitudinal and latitudinal) at the given longitude/latitude angles
    const float DEG2RAD = acos(-1) / 180.0f;

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    tuple<double, double, double> vertices[pointsPerRow][pointsPerRow];

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
        glColor3f(color.r, color.g, color.b);
        /* for(int i = 1; i < pointsPerRow; ++i){
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
        } */

        // Change for online ============================
        auto [x, y, z] = vertices[0][0];
        glVertex3d(x, y, z);

        tie(x, y, z) = vertices[0][pointsPerRow -1];
        glVertex3d(x, y, z);

        tie(x, y, z) = vertices[pointsPerRow-1][pointsPerRow-1];
        glVertex3d(x, y, z);

        tie(x, y, z) = vertices[pointsPerRow-1][0];
        glVertex3d(x, y, z);
        // change for online =============================
    }glEnd();
}

/**
 * @brief Draw a sphere using 6 cubesphere segments
 */
void drawSphere(){
    GLfloat r = 1-sqrt(2)*radius;

    Color colors[] = {
        Color::green(), 
        Color::green(),
        Color::blue(), 
        Color::blue(),
        Color::red(),
        Color::red()};
    // 2d array of centers
    GLfloat centers[6][3] = {
        {r, 0, 0},
        {-r, 0, 0},
        {0, 0, r},
        {0, 0, -r},
        {0, r, 0},
        {0, -r, 0}
    };
    // rotation angles
    int angles[6] = {0, 180, 270, 90, 90, 270};
    // rotation axis
    GLfloat axis[6][3] = {
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 1}
    };

    for (int i = 0; i < 6; i++) {
        glPushMatrix();{
            glTranslatef(centers[i][0], centers[i][1], centers[i][2]);
            glScalef(radius, radius, radius);
            glRotatef(angles[i], axis[i][0], axis[i][1], axis[i][2]);
            drawCubesphere(colors[i]);
            //drawSphere();
        }glPopMatrix();
    }

}

void drawCylinderSegment(double height, double radius, int segments) {
    const float cylinderAngle = 70.5287794f;
    const float cylinderHeight = height;
    const float cylinderRadius = radius;

    const int numSegments = 100;
    const float segmentAngle = cylinderAngle / numSegments;
    const float halfAngleRadians = (cylinderAngle / 2.0) * M_PI / 180.0;

    /* glColor3f(0.5, 0.5, 0.5);  // Gray color
    glBegin(GL_TRIANGLE_STRIP);{
        glColor3f(1,1,0); // Yellow color
        for (int i = 0; i <= numSegments; ++i) {
            float angle = -halfAngleRadians + i * segmentAngle * M_PI / 180.0;
            float x = cylinderRadius * cos(angle);
            float y = cylinderRadius * sin(angle);

            glVertex3f(x, y, -cylinderHeight / 2.0);
            glVertex3f(x, y, cylinderHeight/2);
        }
    }glEnd(); */

    // change for online
    glColor3f(0.5, 0.5, 0.5);  // Gray color
    glBegin(GL_QUADS);{
        glColor3f(1,1,0); // Yellow color
        //for (int i = 0; i <= numSegments; ++i) {
        float angle = -halfAngleRadians + segmentAngle * M_PI / 180.0;
        float x = cylinderRadius * cos(angle);
        float y = cylinderRadius * sin(angle);

        glVertex3f(x, y, -cylinderHeight / 2.0);
        glVertex3f(x, y, cylinderHeight/2);

        angle = +halfAngleRadians + segmentAngle * M_PI / 180.0;
        x = cylinderRadius * cos(angle);
        y = cylinderRadius * sin(angle);

        glVertex3f(x, y, cylinderHeight/2);
        glVertex3f(x, y, -cylinderHeight / 2.0);
    }glEnd();
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

            glRotated(90, 0, 0, 1);
            drawCylinderSegment(h, radius, 100);
        }glPopMatrix();
    }
    
    for(int i = 0; i < 4; ++i){
        glPushMatrix();{
            glRotated(90*i, 0, 1, 0);

            glColor3f(1, 0, 0);
            glTranslated(0, -r/2, r/2);
            glRotated(-45, 1, 0, 0);

            glRotated(-90, 0, 0, 1);
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

    // control viewing (or camera)

    // cross multiplication between (l-eye) and u
    r.x = (look.y)*up.z - (look.z)*up.y;
    r.y = (look.z)*up.x - (look.x)*up.z;
    r.z = (look.x)*up.y - (look.y)*up.x;


    //camera control
    if(isCameraMoving)
        gluLookAt(eye.x,eye.y,eye.z,
            0, 0, 0,
            up.x,up.y,up.z);
    else
    gluLookAt(eye.x,eye.y,eye.z,
            eye.x + look.x ,eye.y + look.y,eye.z + look.z,
            up.x,up.y,up.z);

    // draw
    // rotate everything wrt y axis
    glPushMatrix();{
        //if(isCameraMoving) glTranslatef(0, 1, 0);
        glRotatef(objAngle, 0, 1, 0);

        //drawAxes();
        drawAllCylinder();
        drawOctaHedron();
        drawSphere();
    }glPopMatrix();


    radius = 1/sqrt(2) - radius;
    glPushMatrix();{
        glTranslatef(2, 0, 0);
        glRotatef(objAngle, 1, 0, 0);

        //drawAxes();
        drawAllCylinder();
        drawOctaHedron();
        drawSphere();
    }glPopMatrix();
    radius = 1/sqrt(2) - radius;

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

    // Enable perspective projection with fovy, aspect, zNear and zFar
    // for 3D view
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y) {
    double rate = 0.01;
    isCameraMoving = false;
	switch(key){
		case '1': // rotate/look right
			r.x = r.x*cos(rate)+look.x*sin(rate);
			r.y = r.y*cos(rate)+look.y*sin(rate);
			r.z = r.z*cos(rate)+look.z*sin(rate);

			look.x = look.x*cos(rate)-r.x*sin(rate);
			look.y = look.y*cos(rate)-r.y*sin(rate);
			look.z = look.z*cos(rate)-r.z*sin(rate);
			break;

        case '2': // rotate/look left
			r.x = r.x*cos(-rate)+look.x*sin(-rate);
			r.y = r.y*cos(-rate)+look.y*sin(-rate);
			r.z = r.z*cos(-rate)+look.z*sin(-rate);

			look.x = look.x*cos(-rate)-r.x*sin(-rate);
			look.y = look.y*cos(-rate)-r.y*sin(-rate);
			look.z = look.z*cos(-rate)-r.z*sin(-rate);
			break;

        case '3': // look up
            rate = 0.1;
			look.x = look.x*cos(rate)+up.x*sin(rate);
			look.y = look.y*cos(rate)+up.y*sin(rate);
			look.z = look.z*cos(rate)+up.z*sin(rate);

			up.x = up.x*cos(rate)-look.x*sin(rate);
			up.y = up.y*cos(rate)-look.y*sin(rate);
			up.z = up.z*cos(rate)-look.z*sin(rate);
			break;

        case '4': // look down 
            rate = 0.1;
			look.x = look.x*cos(-rate)+up.x*sin(-rate);
			look.y = look.y*cos(-rate)+up.y*sin(-rate);
			look.z = look.z*cos(-rate)+up.z*sin(-rate);

			up.x = up.x*cos(-rate)-look.x*sin(-rate);
			up.y = up.y*cos(-rate)-look.y*sin(-rate);
			up.z = up.z*cos(-rate)-look.z*sin(-rate);
			break;

        case '5': // tilt counter clockwise
            rate = 0.01;
			up.x = up.x*cos(rate)+r.x*sin(rate);
			up.y = up.y*cos(rate)+r.y*sin(rate);
			up.z = up.z*cos(rate)+r.z*sin(rate);

			r.x = r.x*cos(rate)-up.x*sin(rate);
			r.y = r.y*cos(rate)-up.y*sin(rate);
			r.z = r.z*cos(rate)-up.z*sin(rate);
			break;

        case '6': // tilt clockwise
            rate = 0.01;
			up.x = up.x*cos(-rate)+r.x*sin(-rate);
			up.y = up.y*cos(-rate)+r.y*sin(-rate);
			up.z = up.z*cos(-rate)+r.z*sin(-rate);

			r.x = r.x*cos(-rate)-up.x*sin(-rate);
			r.y = r.y*cos(-rate)-up.y*sin(-rate);
			r.z = r.z*cos(-rate)-up.z*sin(-rate);
			break;
    // Controlling Object
    case 'a':
        objAngle -= 10; // rotate clockwise
        break;
    case 'd':
        objAngle += 10; // rotate anti-clockwise
        break;
    // w - move up without changing reference point
    case 'w':
        eye.y += 0.1;
        look.y -= 0.1;
        isCameraMoving = true;
        break;
    // s - move down without changing reference point
    case 's':
        eye.y -= 0.1;
        look.y += 0.1;
        isCameraMoving = true;
        break;

    
    case '.':
        radius -= 0.045;
        if(radius<0)
            radius = 0;
        break;
    case ',':
        radius += 0.045;
        if(radius>1/sqrt(2))
            radius = 1/sqrt(2);
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
    double rate = 0.1;                          // scaling factor
    switch(key){
		case GLUT_KEY_UP:		//down arrow key
			eye.x+=look.x * rate;
			eye.y+=look.y * rate;
			eye.z+=look.z * rate;
			break;
		case GLUT_KEY_DOWN:		// up arrow key
			eye.x-=look.x * rate;
			eye.y-=look.y * rate;
			eye.z-=look.z * rate;
			break;

		case GLUT_KEY_RIGHT:
            rate = 0.02;
			eye.x+=r.x * rate;
			eye.y+=r.y * rate;
			eye.z+=r.z * rate;
			break;
		case GLUT_KEY_LEFT :
            rate = 0.02;
			eye.x-=r.x * rate;
			eye.y-=r.y * rate;
			eye.z-=r.z * rate;
			break;
		case GLUT_KEY_PAGE_UP:
            rate = 0.1;
		    eye.x+=up.x * rate ;
			eye.y+=up.y * rate;
			eye.z+=up.z * rate;
			break;
		case GLUT_KEY_PAGE_DOWN:
            rate = 0.1;
            eye.x-=up.x * rate;
			eye.y-=up.y * rate;
			eye.z-=up.z * rate;
			break;
    default:
        return;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    eye.x = eye.y =  eye.z = DEFAULT_ZOOM;
    look.x = look.y = look.z = -DEFAULT_ZOOM;
    up.x = up.z = 0; up.y = 1;
    r.x = 1; r.y = r.z = 0;

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