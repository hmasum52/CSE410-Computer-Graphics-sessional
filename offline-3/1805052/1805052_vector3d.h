/**
 * @file vector3d.h
 * @author Hasan Masum (1805052@ugrad.cse.buet.ac.bd)
 * @brief 
 * @version 0.1
 * 
 * 
 */
#pragma once

#include <fstream>
#include <cmath>
using namespace std;

class Vector3D{
public:
    double x;
    double y;
    double z;

    // default constructor
    Vector3D(){
        x = y = z= 0;
    }

    // constructor
    Vector3D(double x, double y, double z){
        this->x = x;this->y = y;this->z = z;
    }

    // copy constructor
    Vector3D(const Vector3D& v){
        x = v.x;y = v.y;z = v.z;
    }

    // assignment operator
    Vector3D& operator=(const Vector3D& v){
        x = v.x;y = v.y;z = v.z;
        return *this;
    }

    // destructor
    ~Vector3D(){}

    // addition
    Vector3D operator+(const Vector3D& v){
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }

    // subtraction
    Vector3D operator-(const Vector3D& v){
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }

    // scalar multiplication
    Vector3D operator*(double s){
        return Vector3D(x * s, y * s, z * s);
    }

    // dot product
    double dot(const Vector3D& v){
        return x * v.x + y * v.y + z * v.z;
    }

    double magnitude(){
        return sqrt(x * x + y * y + z * z);
    }

    // cross product
    Vector3D cross(const Vector3D& v){
        return Vector3D(
            y * v.z - z * v.y,
            z * v.x - x * v.z, 
            x * v.y - y * v.x
        );
    }

    // normalize vector
    Vector3D normalize(){
        double len = sqrt(x * x + y * y + z * z);
        x /= len; y /= len; z /= len;
        return *this;
    }

    // distance between two vectors
    double distance(const Vector3D& v){
        return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
    }

    // angle between two vectors
    double angleWith(const Vector3D& v){
        return acos(this->dot(v) / (sqrt(x * x + y * y + z * z) * sqrt(v.x * v.x + v.y * v.y + v.z * v.z)));
    }

    // friend function
    // vector form of Rodrigues formula
    // param: x is the vector to be rotated
    // param: a is the axis of rotation
    // param: theta is the angle of rotation in radian
    friend Vector3D applyRodriguesFormula(Vector3D& x, Vector3D& a, double theta){
        // R(x, a, theta) = x * cos(theta) +  a * (a dot x)(1 - cos(theta)) + (a cross x)sin(theta)
        return x * cos(theta) + a * (a.dot(x) * (1 - cos(theta))) +  a.cross(x) * sin(theta);
    }

    // print with <<
    friend ostream& operator<<(ostream& os, const Vector3D& v){
        os<<"("<<v.x<<", "<<v.y<<", "<<v.z<<")";
        return os;
    }

    // in the format: x y z
    friend istream& operator>>(istream& is, Vector3D& v){
        is >> v.x >> v.y >> v.z;
        return is;
    }
};

