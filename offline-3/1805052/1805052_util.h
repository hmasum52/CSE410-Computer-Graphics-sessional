#pragma once
#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <fstream>
#include "1805052_vector3d.h"
#include <iostream>
#include <string>
using namespace std;

// open file function with error check
// can open file in both read and write mode
// return a fstream object
fstream open_file(string filename, ios_base::openmode mode = ios_base::in){
    cout<<"Opening file: "<<filename<<endl;
    fstream file;
    file.open(filename, mode);
    if(!file.is_open()){
        cout<<"Error opening file: "<<filename<<endl;
        exit(1);
    }
    return file;
}



void drawSphere(double radius, int stacks, int slices) {
    Vector3D points[stacks+1][slices+1];
    for (int j = 0; j <= stacks; j++) {
        double phi = -M_PI / 2.0 + j * M_PI / stacks;
        double r = radius * cos(phi);
        double h = radius * sin(phi);
        for (int i = 0; i < slices+1; i++) {
            double theta = i * 2.0 * M_PI / slices;
            points[j][i].x = r * cos(theta);
            points[j][i].y = r * sin(theta);
            points[j][i].z = h;
        }
    }

    glBegin(GL_QUADS);
        for (int j = 0; j < stacks; j++) {
            for (int i = 0; i < slices; i++) {
                GLfloat c = (2+cos((i+j) * 2.0 * M_PI / slices)) / 3;
                glColor3f(c,c,c);
                glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
                glVertex3f(points[j][i+1].x, points[j][i+1].y, points[j][i+1].z);

                glVertex3f(points[j+1][i+1].x, points[j+1][i+1].y, points[j+1][i+1].z);
                glVertex3f(points[j+1][i].x, points[j+1][i].y, points[j+1][i].z);
            }
        }
    glEnd();
}

void drawCone(double height, double radius, int segments) {
    double tempx = radius, tempy = 0;
    double currx, curry;
    glBegin(GL_TRIANGLES);
        for (int i = 1; i <= segments; i++) {
            double theta = i * 2.0 * M_PI / segments;
            currx = radius * cos(theta);
            curry = radius * sin(theta);


            GLfloat c = (5+cos(theta))/6;
            glColor3f(c,c,c);
            glVertex3f(0, 0, height/2);
            glVertex3f(currx, curry, -height/2);
            glVertex3f(tempx, tempy, -height/2);

            tempx = currx;
            tempy = curry;
        }
    glEnd();
}
