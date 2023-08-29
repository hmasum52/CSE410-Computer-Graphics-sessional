// base object classes
// Created: 05/05/2005 11:17:52
// by: Hasan Masum, CSE18, BUET

#pragma once

#include <GL/glut.h>
#include <fstream>
#include <cmath>
#include "vector3d.h"
#include "1805052_color.h"
using namespace std;


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
};


class CheckerBoard : public Object{
    Vector3D center;  // center of the board
    int nTile; // number of tiles in each row/column
    double wTile; // width of each tile

public:
    CheckerBoard(){
        center = Vector3D(0, 0, 0);
        wTile = 0;
        nTile = 0;
    }

    // constructor with board width and number of tiles
    CheckerBoard(double wCheckerboard, double wTile){
        center = Vector3D(-wCheckerboard/2, -wCheckerboard/2, 0);
        this->wTile = wTile;
        nTile = (int) wCheckerboard / wTile;
    }

    void draw(){
        double x, y;
        glBegin(GL_QUADS);{
            for(int i=0; i<nTile; i++){
                for(int j=0; j<nTile; j++){
                    x = center.x + i*wTile;
                    y = center.y + j*wTile;

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