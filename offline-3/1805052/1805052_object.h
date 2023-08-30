// base object classes
// Created: 05/05/2005 11:17:52
// by: Hasan Masum, CSE18, BUET

#pragma once

#include <GL/glut.h>
#include <fstream>
#include <cmath>
#include "1805052_vector3d.h"
#include "1805052_color.h"
#include "1805052_ray.h"
using namespace std;

#define INF 1e9


class Object{
protected:
    // double height;
    // double width;
    // double length;
    string name;
    double lightCoefficients[4]; // ambient, diffuse, specular, recursive
    int shininess; // exponent of specular reflection

public:
    Color color; // default color is black

    // default constructor
    Object(){
        name = "";
       // height = width = length = 0;
        lightCoefficients[0] = lightCoefficients[1] = lightCoefficients[2] = lightCoefficients[3] = 0;
        shininess = 0;
    }

    Object(string name){
        this->name = name;
        lightCoefficients[0] = lightCoefficients[1] = lightCoefficients[2] = lightCoefficients[3] = 0;
        shininess = 0;
    }

    string getName(){
        return name;
    }

    void setColor(Color color){
        this->color = color;
    }

    void setShininess(int shininess){
        this->shininess = shininess;
    }

    void setLightCoefficients(double ambient, double diffuse, double specular, double recursive){
        lightCoefficients[0] = ambient;
        lightCoefficients[1] = diffuse;
        lightCoefficients[2] = specular;
        lightCoefficients[3] = recursive;
    }


    virtual void draw() = 0;

    // return the intersection point of the ray with the object
    double intersect(Ray ray){
        return intersectAndIlluminate(ray, color, 0); // color is not used
    }

    Color getColor(Vector3D intersectionPoint){
        return Color(0, 0, 0);
    }

    virtual double intersectAndIlluminate(Ray ray, Color& color, int level) = 0;
    /* {
        return INF;
    } */
};


class CheckerBoard : public Object{
    Vector3D topLeft;  // center of the board
    int nTile; // number of tiles in each row/column
    double wTile; // width of each tile

public:
    CheckerBoard(){
        topLeft = Vector3D(0, 0, 0);
        wTile = 0;
        nTile = 0;
    }

    // constructor with board width and number of tiles
    CheckerBoard(double wCheckerboard, double wTile){
        topLeft = Vector3D(-wCheckerboard/2, -wCheckerboard/2, 0);
        this->wTile = wTile;
        nTile = (int) wCheckerboard / wTile;
    }

    void draw(){
        double x, y;
        glBegin(GL_QUADS);{
            for(int i=0; i<nTile; i++){
                for(int j=0; j<nTile; j++){
                    x = topLeft.x + i*wTile;
                    y = topLeft.y + j*wTile;

                    int color = (i+j)%2 == 0 ? 1 : 0;
                    glColor3f(color, color, color);

                    glVertex3f(x, y, 0);
                    glVertex3f(x+wTile, y, 0);
                    glVertex3f(x+wTile, y+wTile, 0);
                    glVertex3f(x, y+wTile, 0);
                }
            }
        }glEnd();
    }

    Vector3D normal(Ray incidentRay){
        return Vector3D(0, 0, incidentRay.dir.z > 0 ? 1 : -1);
    }

    // color at point p
    Color getColor(Vector3D p){
        int i = (p.x - topLeft.x) / wTile;
        int j = (p.y - topLeft.y) / wTile;
        int color = (i+j)%2 == 0 ? 1 : 0;
        return Color(color, color, color);
    }

    // find intersection point of the ray with the board
    double intersectAndIlluminate(Ray ray, Color& color, int level){
        // get normal
        Vector3D n = normal(ray);

        // check if perpendicular to the plane
        double denom = n.dot(ray.dir);
        if(denom == 0){
            return INF;
        }

        // find the intersection point
        double t = - (n.dot(ray.origin)) / denom; // distance from origin
        Vector3D intersectionPoint = ray.origin + ray.dir*t;

        // check if the intersection point is inside the board
        if(intersectionPoint.x < topLeft.x || intersectionPoint.x > -topLeft.x){
            return INF;
        }

        if(intersectionPoint.y < topLeft.y || intersectionPoint.y > -topLeft.y){
            return INF;
        }

        if(level == 0){
            return t;
        }

        // illuminate the intersection point
       // color = illuminate(intersectionPoint, n, ray.dir);
        color = getColor(intersectionPoint);

        return t;
    }

};

class Sphere : public Object{
    Vector3D center;
    double radius;
public:
    Sphere(){
        center = Vector3D(0, 0, 0);
        radius = 0;
    }

    Sphere(Vector3D ref_point, double radius){
        this->center = ref_point;
        this->radius = radius;
    }

    void draw(){
        glPushMatrix();{
            glTranslatef(center.x, center.y, center.z);
            glColor3f(color.r, color.g, color.b);
            glutSolidSphere(radius, 100, 100);
        }glPopMatrix();
    }

    // find intersection point of the ray with the sphere
    double intersectAndIlluminate(Ray ray, Color& color, int level){
        // P(t) = origin + t*dir
        // H(p) = P*P - r*r = 0
        // R0 = origin - center
        // => dir.dir*t^2 + 2*dir.R0*t + R0.R0 - r*r = 0
        // a = dir*dir
        // b = 2*dir.R0
        // c = R0*R0 - r*r
        
        Vector3D intersectionPoint;
        Vector3D origin = ray.origin;
        Vector3D dir = ray.dir;

        // calculate the coefficients of the quadratic equation
        double a = dir.dot(dir); // dir.dir
        double b = 2 * dir.dot(origin - center); // 2*dir.R0
        double c = (origin - center).dot(origin - center) - radius*radius; // R0.R0 - r*r

        // calculate the discriminant
        double discriminant = b*b - 4*a*c;

        // if discriminant is negative, no intersection
        if(discriminant < 0){
            return INF;
        }

        double tMin = INF; // distance of the closest intersection point

        // if discriminant is zero, one intersection
        if(discriminant == 0){
            tMin = -b / (2*a);
        }
        // if discriminant is positive, two intersections
        else{
            double t1 = (-b + sqrt(discriminant)) / (2*a);
            double t2 = (-b - sqrt(discriminant)) / (2*a);
            tMin = min(t1, t2);
        }
        
        if (level == 0){
            return tMin;
        }

        intersectionPoint = origin + dir*tMin;
       // color = illuminate(intersectionPoint, normal(intersectionPoint), dir);
        color = this->color;

        return tMin;
    }

    friend ostream& operator>>(istream& os, Sphere& c){
        os >> c.center; // center
        os >> c.radius;
        os >> c.color;
        double ambient, diffuse,specular, reflection; 
        os >> ambient >> diffuse>> specular >> reflection;
        c.setLightCoefficients(ambient, diffuse, specular, reflection);
        os >> c.shininess;
    }
};

class Pyramid : public Object{
    Vector3D lowestPoint;
    double height;
    double width;

public:

    Pyramid(){
        lowestPoint = Vector3D(0, 0, 0);
        height = width = 0;
    }

    Pyramid(string name){
        this->name = name;
        lowestPoint = Vector3D(0, 0, 0);
        height = width = 0;
    }

    void draw(){
        glPushMatrix();{
            glTranslatef(lowestPoint.x, lowestPoint.y, lowestPoint.z);
            glColor3f(color.r, color.g, color.b);
            // draw the four sides
            // draw one triangle and rotate it
            glPushMatrix();{
                // position the triangles at the center of the base
                glTranslated(width/2, width/2, 0);
                for(int i=0; i<4; i++){
                    glPushMatrix();{
                        // rotate the triangle to draw the other sides
                        glRotatef(90*i, 0, 0 , 1);
                        glPushMatrix();{
                            // draw a triangle at origin
                            glScalef(width, width, height);
                            glBegin(GL_TRIANGLES);{  // Each set of 3 vertices form a triangle
                                glVertex3f( -0.5f, -0.5f, 0.0f);
                                glVertex3f(0.5f, -0.5f, 0.0f);
                                glVertex3f(0.0f, 0.0f, 1.0f);
                            }glEnd();
                        }glPopMatrix();
                    }glPopMatrix();
                }

                // draw the base at origin
                glBegin(GL_QUADS);{
                    glVertex3f(width/2, width/2, 0);
                    glVertex3f(width/2, -width/2, 0);
                    glVertex3f(-width/2, -width/2, 0);
                    glVertex3f(-width/2, width/2, 0);
                }glEnd();
            }glPopMatrix();
        }glPopMatrix();
    }

    // find intersection point of the ray with the cube
    double intersectAndIlluminate(Ray ray, Color& color, int level){
        return INF;
    }

    friend istream& operator>>(istream& is, Pyramid& c){
        is >> c.lowestPoint;
        is >> c.width >> c.height;
        is >> c.color;
        double ambient, diffuse,specular, reflection; 
        is >> ambient >> diffuse>> specular >> reflection;
        c.setLightCoefficients(ambient, diffuse, specular, reflection);
        is >> c.shininess;
        return is;
    }
};

class Cube : public Object {
    Vector3D bottomLeft; // bottom left corner of the cube
    double side; // length of side
public:
    Cube(){
        bottomLeft = Vector3D(0, 0, 0);
        side = 0;
    }

    Cube(string name){
        this->name = name;
        bottomLeft = Vector3D(0, 0, 0);
        side = 0;
    }

    void draw(){
        // draw side the of cube at center
        // the rotate and tanslate it to draw other sides
        // the finaly translate it to the bottom left corner

        // array of color for each side
        // Color colors[6];
        // colors[0] = Color(1, 0, 0); // red
        // colors[1] = Color(0, 1, 0); // green
        // colors[2] = Color(0, 0, 1); // blue
        // colors[3] = Color(1, 1, 0); // yellow
        // colors[4] = Color(1, 0, 1); // magenta
        // colors[5] = Color(0, 1, 1); // cyan

        glPushMatrix();{
            glTranslatef(bottomLeft.x, bottomLeft.y, bottomLeft.z);
            glColor3f(color.r, color.g, color.b);
            glPushMatrix();{
                glTranslatef(side/2, side/2, side/2);
                for(int i = 0 ; i < 6 ; i++){
                    glPushMatrix();{
                        int angle = i == 4 ? -90 : 90*i;
                        glRotatef(angle, i>=4, i<4 , 0);
                        glPushMatrix();{
                            glTranslated(0, 0, side/2);
                            glScalef(side, side, side);
                            //glColor3f(colors[i].r, colors[i].g, colors[i].b);
                            glBegin(GL_QUADS);{
                                // front
                                glVertex3f(0.5, 0.5, 0);
                                glVertex3f(0.5, -0.5, 0);
                                glVertex3f(-0.5, -0.5, 0);
                                glVertex3f(-0.5, 0.5, 0);
                            }glEnd();
                        }glPopMatrix();
                    }glPopMatrix();
                }
            }glPopMatrix();
        }glPopMatrix();
    }


    // find intersection point of the ray with the cube
    double intersectAndIlluminate(Ray ray, Color& color, int level){
        // P(t) = origin + t*dir
        // need to check if Ray intersects with the cube
        // if it intersects, then find the intersection point
        // and illuminate it

        return INF;
    }


    friend istream& operator>>(istream& is, Cube& c){
        is >> c.bottomLeft;
        is >> c.side;
        is >> c.color;
        double ambient, diffuse,specular, reflection; 
        is >> ambient >> diffuse>> specular >> reflection;
        c.setLightCoefficients(ambient, diffuse, specular, reflection);
        is >> c.shininess;
        return is;
    }
};