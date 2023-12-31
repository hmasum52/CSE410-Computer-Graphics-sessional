// author: Hasan masum, CSE18, BUET

#define _USE_MATH_DEFINES
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>
#include "1805052_vector3d.h"
#include "1805052_util.h"
#include "1805052_color.h"
#include "1805052_object.h"
#include "bitmap_image.hpp"
#include "1805052_light.h"
using namespace std;

// macro for degree to radian conversion
#define DEG2RAD(deg) (deg * M_PI / 180)
#define MOVE_RATE 6
#define ROTATE_UNIT 4*(M_PI/180)
#define INF 1e9
#define fori(i, n) for(int i = 0; i<n; i++)

// inputs from descripton.txt
double nearDist, farDist; // distances of near and far planes
double fovY;                // field of vision along y axis
double aspectRatio;         // aspect ratio of the view window
int nRecurstion;            // recursion level of ray tracing
int nPixels;                // number of pixels in each side of the view window(along both axis)

// opengl parameters
double windowWidth = nPixels;
double windowHeight = nPixels;
vector<vector<Vector3D>> pointBuffer;
CheckerBoard* checkerBoard;

Vector3D eye; 
Vector3D u, r, l;

void init(){
  windowHeight = windowWidth = nPixels;
  
	eye = { 0, -150, 50 };
	u = { 0, 0, 1 }; // up vector
	r = { 1, 0, 0 }; // right vector
	l = { 0, 1,  0 }; // look vector
}


// read description from "description.txt"
void readDescriptionFile(){
  fstream scene = open_file("description.txt", ios_base::in);
  objects.clear();
  lights.clear();
  spotLights.clear();

  // read distanse of near and far plane
  // fov along y axis
  // aspect ratio
  scene>>nearDist>>farDist>>fovY>>aspectRatio;

  // level of recursion
  scene>>nRecurstion;
  // number of pixel in along both axis
  scene>>nPixels;


  // width of each cell of the infinite checkerboard
  int cellWidth;
  scene>>cellWidth;
  // ambient, diffuse and reflection coefficient
  double cofAmbient, cofDiffuse, cofReflection;
  scene>>cofAmbient>>cofDiffuse>>cofReflection;
  checkerBoard = new CheckerBoard(farDist*2, cellWidth);
  checkerBoard->setLightCoefficients(cofAmbient, cofDiffuse, 0, cofReflection);
  checkerBoard->setShininess(30);
  objects.push_back(checkerBoard);

  // number of objects
  int nObjects;
  scene>>nObjects;

  for (int i = 0; i < nObjects; i++){
    string shape; // shape of the object
    scene>>shape;
    //cout<<"shape: "<<shape<<endl;
    if(shape == "sphere"){
      Sphere* s = new Sphere();
      scene>>*s;
      objects.push_back(s);
    
    }else if(shape == "pyramid"){
      Pyramid* p = new Pyramid(shape);
      scene>>*p;
      objects.push_back(p);
    }else if(shape == "cube"){
      Cube* c = new Cube(shape);
      scene>>*c;
      objects.push_back(c);
    }else{
      cout<<"unknown shape: "<<shape<<endl;
      exit(1);
    }
  }

  // read light source description
  int nLightSources, nSpotLightSources;
  scene>>nLightSources;
  for (int i = 0; i < nLightSources; i++){
    NormalLight* light = new NormalLight();
    scene>>*light;
    lights.push_back(light);
  }

  // read spot light source description
  scene>>nSpotLightSources;
  for (int i = 0; i < nSpotLightSources; i++){
    SpotLight* light = new SpotLight();
    scene>>*light;
    spotLights.push_back(light);
  }

}


/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes() {
    glLineWidth(3);
    int length = nPixels*2;
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



void initPointBuffer(){
  // the objects in the real world will be mapped in 
  // screen with
  double height = 2*nearDist*tan(fovY/2);
  double fovX = aspectRatio*fovY;
  double width = 2*nearDist*tan(fovX/2);

  // middle point of the screen
  Vector3D midpoint = eye + l*nearDist; 

  // as now we have the mid point 
  // we use camera r and camera u vector to generate
  // 3d points on the screen
  double stepX = width / nPixels;
  double stepY = height / nPixels;

  pointBuffer.resize(nPixels, std::vector<Vector3D>(nPixels));

  for (int y = 0; y < nPixels; y++) {
      for (int x = 0; x < nPixels; x++) {
          double xOffset = -width / 2 + x * stepX;
          double yOffset = height / 2 - y * stepY;
          Vector3D pointOnNearPlane = midpoint - r * xOffset - u * yOffset;
          pointBuffer[x][y] = pointOnNearPlane;
      }
  }
  cout<<"Point buffer generation done"<<endl;
}

// cast ray
void renderWorld(){
  initPointBuffer();

  // init bitmap image
  bitmap_image image(nPixels, nPixels);
  for(int i = 0; i<nPixels; i++) for(int j = 0; j<nPixels; j++) image.set_pixel(i, j, 0, 0, 0); // set black

  int totalPixels = nPixels*nPixels;
  int prevPercent = 0;

  if(checkerBoard->isTexture()){
    cout<<"Rendering image with texture"<<endl;
  }

  for(int i = 0; i<pointBuffer.size(); i++){
    for(int j = 0; j<pointBuffer[i].size(); j++){
      Vector3D pointOnNearPlane = pointBuffer[i][j];
      Vector3D rayDir = pointOnNearPlane - eye;
    
      // Do not start from the rays from the eye
      // rather, start from the point on the near plane
      // if we start from the eye, we will have to do extra checks to implement the near plane
      // if you start from the point on the near plane, then we only have to 
      // check that thte parameter t > 0
      Ray ray = Ray(eye, rayDir);

      // iterate all object to get the nearest hit point
      double tMin = INF;
      Object *nearestObject = nullptr;
      getNearestIntersectionPoint(ray, tMin, nearestObject);

      if(nearestObject!=nullptr){
        Color pixelColor(0, 0, 0);
        tMin = nearestObject->intersectAndIlluminate(ray, pixelColor, nRecurstion);

        image.set_pixel(i, j, 255*pixelColor.r, 255*pixelColor.g, 255*pixelColor.b);
      }else{
        // no object is intersected
        // set background color
        image.set_pixel(i, j, 0, 0, 0);
      }

      double percent = (i*nPixels + j)*100.0/totalPixels;

      if(int(percent)- (int)prevPercent == 10){
        cout<<"Rendering "<<(int)percent<<"% complete"<<endl; // print progress
        prevPercent = percent;
      }
    }
  }

  cout<<"Rendering 100% complete"<<endl;
  image.save_image("out.bmp");
  cout<<"Image Saved to out.bmp"<<endl;
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
    gluPerspective(fovY, aspectRatio, nearDist, farDist);
}

void rotatePlus(Vector3D&a, Vector3D&b, double angle){
  a.x = a.x * cos(angle) + b.x * sin(angle);
  a.y = a.y * cos(angle) + b.y * sin(angle);
  a.z = a.z * cos(angle) + b.z * sin(angle);
}

void rotateMinus(Vector3D&a, Vector3D&b, double angle){
  a.x = a.x * cos(-angle) - b.x * sin(angle);
  a.y = a.y * cos(-angle) - b.y * sin(angle);
  a.z = a.z * cos(-angle) - b.z * sin(angle);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y) {
    double rate = ROTATE_UNIT;
	switch(key){
    case '0': // caputre bitmap image
      renderWorld();
      break;
		case '1': // rotate/look right
			rotatePlus(r, l, rate);
			rotateMinus(l, r, rate);
			break;

    case '2': // rotate/look left
      rotatePlus(r, l, -rate);
      rotateMinus(l, r, -rate);
			break;

    case '3': // look up
           // rate = 0.1;
			rotatePlus(l, u, rate);
      rotateMinus(u, l, rate);
			break;

    case '4': // look down 
            //rate = 0.1
      rotatePlus(l, u, -rate);
      rotateMinus(u, l, -rate);
			break;

    case '5': // tilt counter clockwise
            //rate = 0.01;
			rotatePlus(r, u, rate);
      rotateMinus(u, r, rate);
			break;

     case '6': // tilt clockwise
           // rate = 0.01;
			rotatePlus(r, u, -rate);
      rotateMinus(u, r, -rate);
			break;

    // pressing space enable texture in checkerboard
    case ' ': // toggle texture
      cout<<(checkerBoard->isTexture() ? "Texture disabled" : "Texture enabled") <<endl;
      checkerBoard->toggleTexture();
      break;

    case 'r':
    case 'R':
      // turn off reflection
      cout<<"Reflection mode toggled"<<endl;
      for(auto o: objects){
        o->toggleReflectionEnabled();
      }
      break;
    case 's':
    case 'S':
      // toggle spot light
      cout<<"Spot light mode toggled"<<endl;
      for(auto o: objects){
        o->toggleSpotLightEnabled();
      }
      break;
    case 'n':
    case 'N':
      // toggle normal light
      cout<<"Normal light mode toggled"<<endl;
      for(auto o: objects){
        o->toggleNormalLightEnabled();
      }
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
    Vector3D change(0, 0, 0);
    switch(key){
		case GLUT_KEY_UP:		//down arrow key
      change = l * rate;

			eye = eye + l * rate;
			break;
		case GLUT_KEY_DOWN:		// up arrow key
      change = (Vector3D(0,0,0) -l) * rate;
			eye = eye - l * rate;
			break;
		case GLUT_KEY_RIGHT:
      change = r * rate;
      eye = eye + r * rate;
			break;
		case GLUT_KEY_LEFT :
      change = (Vector3D(0,0,0) -r) * rate;
      eye = eye - r * rate;
			break;
		case GLUT_KEY_PAGE_UP:
      change = u * rate;
      eye = eye + u * rate;
			break;
		case GLUT_KEY_PAGE_DOWN:
      change = (Vector3D(0,0,0) -u) * rate;
      eye = eye - u * rate;
			break;
    case GLUT_KEY_INSERT:
        readDescriptionFile();
        break;
    default:
        return;
    }
    
    // for infinite checkerboard
    checkerBoard->addToTopLeft(
      change.x, change.y, 0
    );
  
    glutPostRedisplay();    // Post a paint request to activate display()
}

void display(){
  //clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // load the MODELVIEW matrix
  glMatrixMode(GL_MODELVIEW);

  // initialize the matrix
  glLoadIdentity();

   // cross multiplication between (l-eye) and u
  r = l.cross(u);

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
      o->draw();
    }
    // draw the normal light sources
    for(auto light: lights){
      light->draw();
    }

    // draw the spot light sources
    for(auto light: spotLights){
      light->draw();
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
    readDescriptionFile(); // read description from "description.txt"
    cout<<"Done taking input"<<endl;

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