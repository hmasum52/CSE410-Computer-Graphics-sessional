#pragma once
#include "1805052_vector3d.h"

class Ray{
public:
    Vector3D origin;
    Vector3D dir;

    Ray(){
        origin = Vector3D();
        dir = Vector3D();
    }

    Ray(Vector3D origin, Vector3D dir){
        this->origin = origin;
        this->dir = dir;
        this->dir.normalize(); // normalize for easier calculation.
    }
};