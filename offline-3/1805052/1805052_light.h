#pragma once
#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <fstream>
#include "1805052_vector3d.h"
#include "1805052_color.h"
using namespace std;

#define DEG2RAD(deg) (deg * M_PI / 180)
#define RAD2DEG(rad) (rad * 180 / M_PI)

/* class Light{
protected:
    Vector3D position;
    // falllout after every recursion
    double fallout; // 1.0 for no fallout
    // color of the light
public:
  virtual void draw() = 0 ;
}; */


class NormalLight{
protected:
    Vector3D position;
    // falllout after every recursion
    double fallout; // 1.0 for no fallout
public:

  void draw(){
    glPushMatrix();
    // gray color
    glColor3f(0.5, 0.5, 0.5);
    glTranslated(position.x, position.y, position.z);
    int slices = 30;
    //glutSolidSphere(5,  slices, slices);
    drawSphere(5, slices, slices);
    glPopMatrix();
  }

  // friend function to read light from istream
  friend istream& operator>>(istream& in, NormalLight& light){
    in >> light.position >> light.fallout;
    cout<<"Normal Light position: "<<light.position<<endl;
    return in;
  }
};

class SpotLight:NormalLight {
private:
    Vector3D direction;
    double cutOffAngle; // in radian
public:
  // draw a cone 
  void draw(){
    // drawa the spot light as a cone in
    // the direction of the light
    glPushMatrix();

    glTranslated(position.x, position.y, position.z);

    // rotate the cone such a way that it's tip is in the direction of the light
    Vector3D up(0, 0, 1);
    Vector3D axis = up.cross(direction);
    double angle = acos(up.dot(direction));
    glRotated(RAD2DEG(angle+180), axis.x, axis.y, axis.z);

    // move up
    int height = 35;
    //glTranslated(0, 0, height/2);
    drawCone(height, 5, 30);
    //glutSolidCone(5, 20, 30, 30);
    glPopMatrix();
  }

  // friend function to read light from istream
  friend istream& operator>>(istream& in, SpotLight& light){
    in >> light.position >> light.fallout >> light.direction >> light.cutOffAngle;
    light.direction.normalize();
    DEG2RAD(light.cutOffAngle);
    cout<<"Spot Light position: "<<light.position<<endl;
    return in;
  }

};
