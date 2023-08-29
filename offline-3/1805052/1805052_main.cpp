// author: Hasan masum, CSE18, BUET

#define _USE_MATH_DEFINES
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>
#include "vector3d.h"
#include "1805052_util.h"
#include "1805052_color.h"
#include "1805052_object.h"
using namespace std;

// macro for degree to radian conversion
#define DEG2RAD(deg) (deg * M_PI / 180)
#define MOVE_RATE 4
#define ROTATE_UNIT 3*(M_PI/180)

// inputs from descripton.txt
double nearPlane, farPlane; // distances of near and far planes
double fovY;                // field of vision along y axis
double aspectRatio;         // aspect ratio of the view window
int nRecurstion;            // recursion level of ray tracing
int nPixels;                // number of pixels in each side of the view window(along both axis)
int cellWidth;              // width of each chell of the infinite checkerboard
double cofAmbient;             // ambient coefficient
double cofDiffuse;             // diffuse coefficient
double cofReflection;          // reflection coefficient
int nObjects;                  // number of objects in the scene
// ---
// object desction
CheckerBoard checkerBoard;  // checker board
vector<Object*> objects;    // objects in the scene
// ---
// light source description
int nLightSources;          // number of light sources
Vector3D lightSource;       // position of the light source
double srcfalloff;             // falloff factor
// ---
// spot light source description
int nSpotLightSources;      // number of spot light sources
Vector3D spotLightSource;   // position of the spot light source
double spotLightFallOff;       // falloff factor
Vector3D spotLightDir;      // direction of the spot light source(point to which it is looking)
double cutOffAngle;         // cut off angle in degree

// opengl parameters
double cameraHeight;
double cameraAngle;
double viewAngle = 80;
double angle;
double windowWidth = 700;
double windowHeight = 700;

Vector3D eye; 
Vector3D u, r, l;

void init(){
	//codes for initialization
	cameraHeight=80.0;
	cameraAngle=1.0;
	angle=0;

	eye = { 0, -150, 64 };
	u = { 0, 0, 1 }; // up vector
	r = { 1, 0, 0 }; // right vector
	l = { 0, 1,  0 }; // look vector
}

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes() {
    glLineWidth(3);
    int length = cameraHeight*2;
    glBegin(GL_LINES);
        glColor3f(1,0,0);   // Red
        // X axis
        glVertex3f(0,0,0);
        glVertex3f(length,0,0);

        glColor3f(0,1,0);   // Green
        // Y axis
        glVertex3f(0,0,0);
        glVertex3f(0,length,0);

        glColor3f(0,0,1);   // Blue
        // Z axis
        glVertex3f(0,0,0);
        glVertex3f(0,0,length);
    glEnd();
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
    gluPerspective(fovY, aspectRatio, nearPlane, farPlane);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y) {
    double rate = ROTATE_UNIT;
	switch(key){
		case '1': // rotate/look right
			r.x = r.x*cos(rate)+l.x*sin(rate);
			r.y = r.y*cos(rate)+l.y*sin(rate);
			r.z = r.z*cos(rate)+l.z*sin(rate);

			l.x = l.x*cos(rate)-r.x*sin(rate);
			l.y = l.y*cos(rate)-r.y*sin(rate);
			l.z = l.z*cos(rate)-r.z*sin(rate);
			break;

        case '2': // rotate/look left
			r.x = r.x*cos(-rate)+l.x*sin(-rate);
			r.y = r.y*cos(-rate)+l.y*sin(-rate);
			r.z = r.z*cos(-rate)+l.z*sin(-rate);

			l.x = l.x*cos(-rate)-r.x*sin(-rate);
			l.y = l.y*cos(-rate)-r.y*sin(-rate);
			l.z = l.z*cos(-rate)-r.z*sin(-rate);
			break;

        case '3': // look up
           // rate = 0.1;
			l.x = l.x*cos(rate)+u.x*sin(rate);
			l.y = l.y*cos(rate)+u.y*sin(rate);
			l.z = l.z*cos(rate)+u.z*sin(rate);

			u.x = u.x*cos(rate)-l.x*sin(rate);
			u.y = u.y*cos(rate)-l.y*sin(rate);
			u.z = u.z*cos(rate)-l.z*sin(rate);
			break;

        case '4': // look down 
            //rate = 0.1;
			l.x = l.x*cos(-rate)+u.x*sin(-rate);
			l.y = l.y*cos(-rate)+u.y*sin(-rate);
			l.z = l.z*cos(-rate)+u.z*sin(-rate);

			u.x = u.x*cos(-rate)-l.x*sin(-rate);
			u.y = u.y*cos(-rate)-l.y*sin(-rate);
			u.z = u.z*cos(-rate)-l.z*sin(-rate);
			break;

        case '5': // tilt counter clockwise
            //rate = 0.01;
			u.x = u.x*cos(rate)+r.x*sin(rate);
			u.y = u.y*cos(rate)+r.y*sin(rate);
			u.z = u.z*cos(rate)+r.z*sin(rate);

			r.x = r.x*cos(rate)-u.x*sin(rate);
			r.y = r.y*cos(rate)-u.y*sin(rate);
			r.z = r.z*cos(rate)-u.z*sin(rate);
			break;

        case '6': // tilt clockwise
           // rate = 0.01;
			u.x = u.x*cos(-rate)+r.x*sin(-rate);
			u.y = u.y*cos(-rate)+r.y*sin(-rate);
			u.z = u.z*cos(-rate)+r.z*sin(-rate);

			r.x = r.x*cos(-rate)-u.x*sin(-rate);
			r.y = r.y*cos(-rate)-u.y*sin(-rate);
			r.z = r.z*cos(-rate)-u.z*sin(-rate);
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
    double rate = MOVE_RATE;                          // scaling factor
    switch(key){
		case GLUT_KEY_UP:		//down arrow key
			eye.x+=l.x * rate;
			eye.y+=l.y * rate;
			eye.z+=l.z * rate;
			break;
		case GLUT_KEY_DOWN:		// up arrow key
			eye.x-=l.x * rate;
			eye.y-=l.y * rate;
			eye.z-=l.z * rate;
			break;

		case GLUT_KEY_RIGHT:
           // rate = 0.02;
			eye.x+=r.x * rate;
			eye.y+=r.y * rate;
			eye.z+=r.z * rate;
			break;
		case GLUT_KEY_LEFT :
            //rate = 0.02;
			eye.x-=r.x * rate;
			eye.y-=r.y * rate;
			eye.z-=r.z * rate;
			break;
		case GLUT_KEY_PAGE_UP:
            //rate = 0.1;
		    eye.x+=u.x * rate ;
			eye.y+=u.y * rate;
			eye.z+=u.z * rate;
			break;
		case GLUT_KEY_PAGE_DOWN:
           // rate = 0.1;
            eye.x-=u.x * rate;
			eye.y-=u.y * rate;
			eye.z-=u.z * rate;
			break;
    default:
        return;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

// read description from "description.txt"
void readDescription(){
  fstream scene = open_file("description.txt", ios_base::in);

  string blank; // to read blank lines

  // read distanse of near and far plane
  // fov along y axis
  // aspect ratio
  scene>>nearPlane>>farPlane>>fovY>>aspectRatio;

  // level of recursion
  scene>>nRecurstion;
  // number of pixel in along both axis
  scene>>nPixels;


  // width of each cell of the infinite checkerboard
  scene>>cellWidth;
  // ambient, diffuse and reflection coefficient
  scene>>cofAmbient>>cofDiffuse>>cofReflection;
  checkerBoard = CheckerBoard(farPlane, cellWidth);
  objects.push_back(new CheckerBoard(farPlane, cellWidth));

  // number of objects
  scene>>nObjects;

  for (int i = 0; i < nObjects; i++){
    string shape; // shape of the object
    scene>>shape;
    cout<<"shape: "<<shape<<endl;
    if(shape == "sphere"){
      // Vector3D center;
      // scene>>center.x>>center.y>>center.z;
      // double radius; scene >> radius;
      // Color color; scene >> color.r >> color.g >> color.b;
      // double ambient, diffuse,specular, reflection; 
      // scene >> ambient >> diffuse>> specular >> reflection;
      // double shininess; scene >> shininess;
      Sphere* s = new Sphere();
      scene>>*s;
      objects.push_back(s);
    
    }else if(shape == "pyramid"){
      // Vector3D lowestPoint;
      // scene>>lowestPoint.x>>lowestPoint.y>>lowestPoint.z;
      // double width, height; scene >> width >> height;
      // Color color; scene >> color.r >> color.g >> color.b;
      // double ambient, diffuse,specular, reflection;
      // scene >> ambient >> diffuse>> specular >> reflection;
      // double shininess; scene >> shininess;
      Pyramid* p = new Pyramid(shape);
      scene>>*p;
      objects.push_back(p);
    }else if(shape == "cube"){
      /* Vector3D bottomLeft;
      scene>>bottomLeft.x>>bottomLeft.y>>bottomLeft.z;
      double side; scene >> side; cout<<side<<endl;
      Color color; scene >> color.r >> color.g >> color.b;
      double ambient, diffuse,specular, reflection;
      scene >> ambient >> diffuse>> specular >> reflection;
      double shininess; scene >> shininess; */
      Cube* c = new Cube(shape);
      scene>>*c;
      objects.push_back(c);
    }else{
      cout<<"unknown shape: "<<shape<<endl;
      exit(1);
    }
    cout<<endl;
  }

  // read light source description
  scene>>nLightSources;
  for (int i = 0; i < nLightSources; i++){
    scene>>lightSource.x>>lightSource.y>>lightSource.z;
    scene>>srcfalloff;
  }

  // read spot light source description
  scene>>nSpotLightSources;
  for (int i = 0; i < nSpotLightSources; i++){
    scene>>spotLightSource.x>>spotLightSource.y>>spotLightSource.z;
    scene>>spotLightFallOff;
    scene>>spotLightDir.x>>spotLightDir.y>>spotLightDir.z;
    scene>>cutOffAngle;
  }

}

void display(){
  //clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // load the MODELVIEW matrix
  glMatrixMode(GL_MODELVIEW);

  // initialize the matrix
  glLoadIdentity();

   // cross multiplication between (l-eye) and u
    r.x = (l.y)*u.z - (l.z)*u.y;
    r.y = (l.z)*u.x - (l.x)*u.z;
    r.z = (l.x)*u.y - (l.y)*u.x;



  // set the camera
  gluLookAt(
    eye.x, eye.y, eye.z, 
    eye.x + l.x, eye.y + l.y, eye.z + l.z, 
    u.x, u.y, u.z
  );

  // Draw axes
  glPushMatrix();{
    drawAxes();
    //checkerBoard.draw();
    for(auto o : objects){
      //if(o->getName() == "pyramid")
      //  o->draw();
      o->draw();
    }
  }
  glPopMatrix();


  // draw
  glutSwapBuffers();

}

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
}

int main(int argc, char** argv) {
    cout<<"Reading input"<<endl;
    readDescription(); // read description from "description.txt"
    cout<<"Input read done"<<endl;

    init(); // set drawing parameters

    glutInit(&argc, argv);                         // Initialize GLUT
    glutInitWindowSize(windowWidth, windowHeight); // Set the window's initial width & height
    glutInitWindowPosition(50, 50);                // Position the window's initial top-left corner
	  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");         // Create a window with the given title
    glutDisplayFunc(display);                      // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);              // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);            // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);           // Register callback handler for special-key event
    initGL();                                      // Our own OpenGL initialization
    glutMainLoop();                                // Enter the event-processing loop
    return 0;
}