// base object classes
// Created: 05/05/2005 11:17:52
// by: Hasan Masum, CSE18, BUET

#pragma once

#include <GL/glut.h>
#include <fstream>
#include <cmath>
#include <vector>
#include "1805052_vector3d.h"
#include "1805052_color.h"
#include "1805052_ray.h"
#include "1805052_light.h"
#include "bitmap_image.hpp"
using namespace std;

#define INF 1e9

class Object{
protected:
    string name;
    double ambient, diffuse, specular, reflection; // light coefficients
    int shininess; // exponent of specular reflection

public:
    Color color; // default color is black

    // default constructor
    Object(){
        name = "";
        ambient = diffuse = specular = reflection = 0;
        shininess = 0;
    }

    Object(string name){
        this->name = name;
        ambient = diffuse = specular = reflection = 0;
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

    void setLightCoefficients(
        double ambient, 
        double diffuse, 
        double specular, 
        double recursive){
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->reflection = recursive;
    }

    void applyNormalLights(double& intensity, Vector3D normal, Vector3D incidentRay){
        intensity = intensity + diffuse * max(0.0, normal.dot(incidentRay));
    }

    // illuminate 
    Color illuminate(Vector3D p, Vector3D normal, Vector3D incidentRay, int level){
        Color c(0, 0, 0);

        // ambient
        c = c + getColor(p) *ambient;

        // diffuse
        applyDiffuseAndSpecularReflection(c, p, normal, incidentRay, level);

        // reflected ray
        return c;
    }

    // apply diffuse and specular reflection
    void applyDiffuseAndSpecularReflection(Color &c, Vector3D p, Vector3D normal, Vector3D incidentRay, int level);

    // apply spoit lights
    void applySpotLights(double& intensity, Vector3D normal, Vector3D incidentRay);

    virtual void draw() = 0;

    // return the intersection point of the ray with the object
    double intersect(Ray ray){
        return intersectAndIlluminate(ray, color, 0); // color is not used
    }

    virtual Color getColor(Vector3D intersectionPoint) =  0;

    // normal at a point
    virtual Vector3D normal(Vector3D intersectionPoint) = 0;

    virtual double intersectAndIlluminate(Ray ray, Color& color, int level) = 0;
};



class CheckerBoard : public Object{
    Vector3D topLeft;  // center of the board
    int nTile; // number of tiles in each row/column
    double wTile; // width of each tile
    vector<vector<Color>> texture_b; // texture for black color cell
    vector<vector<Color>> texture_w; // texture for white color cell
    bool texture = false;

public:
    CheckerBoard(){
        topLeft = Vector3D(0, 0, 0);
        wTile = 0;
        nTile = 0;
    }

    // function to read a bmp file 
    // pixel by pixel
    vector<vector<Color>> read_texture(string filename){
        bitmap_image* image = new bitmap_image(filename);
        vector<vector<Color>> texture;

        if (!image) {
            cout << "Error - Failed to open: " << filename << endl;
            return texture;
        }

        unsigned int height = image->height();
        unsigned int width = image->width();

        texture.resize(height);
        for(int i=0; i<height; i++){
            texture[i].resize(width);
        }

        unsigned char r, g, b;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++){
                image->get_pixel(j, i, r, g, b);
                texture[i][j] = Color((int)r/255.0, (int)g/255.0, (int)b/255.0);
                //cout<<texture[i][j]<<endl;
            }
        }

        delete image;

        cout<<"Texture read from "<<filename<<endl;
        cout<<"size: "<< height << " " << width << endl;

        return texture;
    }

    void setTexture(bool texture){
        this->texture = texture;
    }

    void toggleTexture(){
        texture = !texture;
    }

    bool isTexture(){
        return texture;
    }

    // constructor with board width and number of tiles
    CheckerBoard(double wCheckerboard, double wTile, bool texture = false){
        topLeft = Vector3D(-wCheckerboard/2, -wCheckerboard/2, 0);
        this->wTile = wTile;
        nTile = (int) wCheckerboard / wTile;
        this->texture = texture;

        texture_w = read_texture("texture_w.bmp");
        //texture_w = read_texture("../Assignment-RayTracer/mushfiq.bmp");
        texture_b = read_texture("texture_b.bmp");
    }

    void draw(){
        if(texture){
            return;
        }
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



    Vector3D normal(Vector3D intersectionPoint){
        return Vector3D(0, 0, intersectionPoint.z> 0 ? 1 : -1);
    }

    Color getTextureColor(Vector3D p){
        int i = (p.x - topLeft.x) / wTile;
        int j = (p.y - topLeft.y) / wTile;

        int x = (int) (i*wTile);
        int y = (int) (j*wTile);

        double px = (p.x - topLeft.x) - x;
        double py = (p.y - topLeft.y) - y;
    
        if((i+j)%2 == 0){
            int pixel_x = (int) (px/wTile * (texture_b.size()-1));
            int pixel_y = (int) (py/wTile * (texture_b.size()-1));
            //cout<<"black "<<i<<" "<<j<<" "<<texture_b[i][j]<<endl;
            return texture_b[pixel_y][pixel_x];
        }
        else{
            int pixel_x = (int) (px/wTile * (texture_w.size()-1));
            int pixel_y = (int) (py/wTile * (texture_w.size()-1));      
            return texture_w[pixel_y][pixel_x];
        }
    }

    // color at point p
    Color getColor(Vector3D p){
        if(texture){
            return getTextureColor(p);
        }

        int i = (p.x - topLeft.x) / wTile;
        int j = (p.y - topLeft.y) / wTile;
        int color = (i+j)%2 == 0 ? 1 : 0;
        return Color(color, color, color);

    }

    // find intersection point of the ray with the board
    double intersectAndIlluminate(Ray ray, Color& color, int level){
        // get normal
        Vector3D n = Vector3D(0, 0, 1);

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

        color = illuminate(intersectionPoint, n, ray.dir, level);
        //color = getColor(intersectionPoint);

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

    // get the normal
    Vector3D normal(Vector3D intersectionPoint){
        return (intersectionPoint - center).normalize();
    }

    // get color at an intersecting point
    Color getColor(Vector3D intersectionPoint){
        return color;
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
        color = illuminate(intersectionPoint, normal(intersectionPoint), dir, level);
        //color = this->color;

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


    // This implementation uses the Möller–Trumbore intersection algorithm 
    // to determine if a ray intersects with a triangle. 
    bool intersectTriangle(const Ray& ray, Vector3D& v0, Vector3D& v1, Vector3D& v2,double& t) {
        Vector3D edge1 = v1 - v0;
        Vector3D edge2 = v2 - v0;
        Vector3D rayOrigin = ray.origin;
        Vector3D rayDir = ray.dir;

        Vector3D h = rayDir.cross(edge2);
        double a = edge1.dot(h);

        // epsilon value is used for numerical stability and 
        // to handle cases where the ray is very close to the triangle.
        const double epsilon = 0.001;

        if (a > -epsilon && a < epsilon) {
            return false; // Ray is parallel to the triangle
        }

        double f = 1.0 / a;
        Vector3D s = rayOrigin - v0;
        double u = f * s.dot(h);

        if (u < 0.0 || u > 1.0) {
            return false;
        }

        Vector3D q = s.cross(edge1);
        double v = f * rayDir.dot(q);

        if (v < 0.0 || u + v > 1.0) {
            return false;
        }

        t = f * edge2.dot(q);

        if (t > epsilon) {
            return true; // Ray intersection
        }

        return false;
    }

    /*
    In this implementation, I'm assuming the square is aligned with the XY plane and centered at the 
    given center point. The function first calculates the plane equation of the square's 
    plane and finds the intersection parameter t. 
    Then, it checks if the intersection point lies within the bounds of the square.
    */
    bool intersectRaySquare(const Ray& ray, double& t) {
        double halfSide = width / 2.0;
        Vector3D normal(0, 0, 1); // Assuming the square lies in the XY plane
        Vector3D rayOrigin = ray.origin;
        Vector3D rayDir = ray.dir;

        // Calculate the plane's distance from the origin
        double d = -normal.dot(lowestPoint); // negative because the normal points in the opposite direction

        double denominator = normal.dot(rayDir);
        
        if (denominator == 0) {
            // Ray is parallel to the plane
            return false;
        }

        t = -(normal.dot(rayOrigin) + d) / denominator;

        if (t < 0) {
            // Intersection behind the ray's origin
            return false;
        }

        Vector3D intersectionPoint = rayOrigin + rayDir * t;

        // Check if the intersection point is within the square's bounds
        double xDist = fabs(intersectionPoint.x - lowestPoint.x);
        double yDist = fabs(intersectionPoint.y - lowestPoint.y);

        if (xDist > halfSide || yDist > halfSide) {
            return false;
        }

        return true;
    }


    bool intersectRayPyramid(
        const Ray& ray, double& distance, Vector3D& normal) {
        Vector3D rayOrigin = ray.origin;
        Vector3D rayDir = ray.dir;
        Vector3D v0 = lowestPoint;
        Vector3D v1 = lowestPoint + Vector3D(width, 0, 0);
        Vector3D v2 = lowestPoint + Vector3D(width, width, 0);
        Vector3D v3 = lowestPoint + Vector3D(0, width, 0);
        vector<Vector3D> vertices = { v0, v1, v2, v3 };
        Vector3D apex = lowestPoint + Vector3D(width / 2, width / 2, height);



        // Check for intersection with the pyramid's base
        double tMin = INF;
        double t;
        bool intersected = intersectRaySquare(ray, t);
        if (intersected) {
            tMin = t;
            normal = Vector3D(0, 0, -1);
        }

        // Check for intersection with each triangular face
        for (int i = 0; i < 4; i++) {
            Vector3D v0 = vertices[i];
            Vector3D v1 = vertices[(i + 1) % 4];
            Vector3D v2 = apex;
            
            if(intersectTriangle(ray, v0, v1, v2, t) && t < tMin) {
                intersected = true;
                distance = t;
                normal = (v1 - v0).cross(v2 - v0).normalize();
            }
        }

        return intersected;
    }
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

    // normal at a point
    Vector3D normal(Vector3D intersectionPoint){
        Vector3D n(0, 0, 0);
        if(intersectionPoint.z == lowestPoint.z){
            n = Vector3D(0, 0, -1);
        }
        else if(intersectionPoint.z == lowestPoint.z + height){
            n = Vector3D(0, 0, 1);
        }
        else{
            Vector3D v0 = lowestPoint;
            Vector3D v1 = lowestPoint + Vector3D(width, 0, 0);
            Vector3D v2 = lowestPoint + Vector3D(width, width, 0);
            Vector3D v3 = lowestPoint + Vector3D(0, width, 0);
            vector<Vector3D> vertices = { v0, v1, v2, v3 };
            Vector3D apex = lowestPoint + Vector3D(width / 2, width / 2, height);

            for (int i = 0; i < 4; i++) {
                Vector3D v0 = vertices[i];
                Vector3D v1 = vertices[(i + 1) % 4];
                Vector3D v2 = apex;

                Vector3D edge1 = v1 - v0;
                Vector3D edge2 = v2 - v0;
                n = edge1.cross(edge2);
                if (n.dot(intersectionPoint - v0) > 0) {
                    break;
                }
            }
        }
        return n;
    }

    Color getColor(Vector3D intersectionPoint){
        return color;
    }

    // find intersection point of the ray with the cube
    double intersectAndIlluminate(Ray ray, Color& color, int level){
        
        Vector3D intersectionPoint;
        double tMin = INF;
        Vector3D n;

        if(intersectRayPyramid(ray,tMin, n)){
            if(level == 0){
                return tMin;
            }
            intersectionPoint = ray.origin + ray.dir*tMin;
            // illuminate the intersection point
            color = illuminate(intersectionPoint, n, ray.dir, level);
            //color = this->color;
            return tMin;
        }

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


    bool intersectRayCube(const Ray& ray, Vector3D& intersectionPoint, double& distance) {
        Vector3D p = this->bottomLeft - ray.origin;
        double tMin = (p.x) / ray.dir.x;
        double tMax = (p.x + this->side) / ray.dir.x;

        if (tMin > tMax) {
            swap(tMin, tMax);
        }

        double tyMin = (p.y) / ray.dir.y;
        double tyMax = (p.y + this->side) / ray.dir.y;

        if (tyMin > tyMax) {
            swap(tyMin, tyMax);
        }

        if ((tMin > tyMax) || (tyMin > tMax)) {
            return false;
        }

        tMin = max (tMin, tyMin);
        tMax = min (tMax, tyMax);
        

        double tzMin = (p.z) / ray.dir.z;
        double tzMax = (p.z + this->side) / ray.dir.z;

        if (tzMin > tzMax) {
            std::swap(tzMin, tzMax);
        }

        if ((tMin > tzMax) || (tzMin > tMax)) {
            return false;
        }

        if (tzMin > tMin) {
            tMin = tzMin;
        }

        if (tzMax < tMax) {
            tMax = tzMax;
        }
        tMin = max(tMin, tzMin);
        tMax = min(tMax, tzMax);

        if (tMin < 0 || tMax < tMin) {
            return false;
        }

        Vector3D origin = ray.origin;
        Vector3D dir = ray.dir;
        intersectionPoint = origin + dir * tMin;
        distance = tMin;
        return true;
    }


    Vector3D normal(Vector3D intersectionPoint){
        Vector3D n(0, 0, 0);
        const double epsilon = 0.001; // Adjust epsilon based on your scene's scale

        if (abs(intersectionPoint.x - bottomLeft.x) < epsilon) {
            n = Vector3D(-1, 0, 0);
        }
        else if (abs(intersectionPoint.x - (bottomLeft.x + side)) < epsilon) {
            n = Vector3D(1, 0, 0);
        }
        else if (abs(intersectionPoint.y - bottomLeft.y) < epsilon) {
            n = Vector3D(0, -1, 0);
        }
        else if (abs(intersectionPoint.y - (bottomLeft.y + side)) < epsilon) {
            n = Vector3D(0, 1, 0);
        }
        else if (abs(intersectionPoint.z - bottomLeft.z) < epsilon) {
            n = Vector3D(0, 0, -1);
        }
        else if (abs(intersectionPoint.z - (bottomLeft.z + side)) < epsilon) {
            n = Vector3D(0, 0, 1);
        }
        return n;
    }

    Color getColor(Vector3D intersectionPoint){
        return color;
    }

    // find intersection point of the ray with the cube
    double intersectAndIlluminate(Ray ray, Color& color, int level){
        double tMin = INF;

        Vector3D intersectionPoint;
        
        if(intersectRayCube(ray, intersectionPoint, tMin)){
            if(level == 0){
                return tMin;
            }
            // illuminate the intersection point
            color = illuminate(intersectionPoint, normal(intersectionPoint), ray.dir, level);
            //color = this->color;
            return tMin;
        }

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


// ---
vector<Object*> objects;    // objects in the scene
// ---
// light source description      // falloff factor
vector<NormalLight*> lights; // light sources in the scene
// ---
// spot light source description
vector<SpotLight*> spotLights; // spot light sources in the scene

void getNearestIntersectionPoint(Ray ray, double& tMin, Object*& nearestObject){
    tMin = INF;

    double eps = 0.001; // to avoid self intersection

    for(int i=0; i<objects.size(); i++){
        double t = objects[i]->intersect(ray);
        if(t < tMin && t > eps){
            tMin = t;
            nearestObject = objects[i];
        }
    }
}

void Object::applyDiffuseAndSpecularReflection(
    Color &c, 
    Vector3D p, // intersection point
    Vector3D normal, 
    Vector3D incidentRay, int level){
    /*
    lambert = phong = 0
    For all sources S:
        if S does not illuminate intersecting point P: continue
        vector toSource = PS;
        toSource.normalize();
        N = normal at intersecting point P
        N.normalize();
        distance = distance between intersecting point P and source S 
        scaling_factor = exp(-distance*distance*S.falloff);
        lambert += toSource.dot(N)*scaling_factor;
        R’ = reflected ray at point P (use N here as well) [here,
        original ray is starting from camera, not from source]
        R’.normalize()
        phong += pow( R’.dot(toSource), shininess)*scaling_factor

    Diffuse component is found as: d*lambert *red, d*lambert*green, d* lambert*blue
    Specular component is found as: s*phong *red, s* phong *green, s* phong *blue
    */

   // reflected ray
    Vector3D reflectedRay = incidentRay - normal * 2 * incidentRay.dot(normal);
    reflectedRay.normalize();

    double lambert = 0, phong = 0;

    for(int i=0; i<lights.size(); i++){
        // check if the light source illuminates the point
        // make a ray from light to the point
        Ray lightRay(lights[i]->position, p - lights[i]->position);
        double tMin = INF;
        Object* nearestObject = NULL;
        getNearestIntersectionPoint(lightRay, tMin, nearestObject);
        
        // if the light source does not illuminate the point, continue
        if(nearestObject == NULL){
            continue;
        }

        if(nearestObject != NULL && nearestObject != this){
            continue;
        }

        // vector to source
        Vector3D toSource = lights[i]->position - p;
        toSource.normalize();

        // distance between intersecting point and source
        double distance = (lights[i]->position - p).magnitude();

        // scaling factor
        double scalingFactor = exp(-distance*distance*lights[i]->falloff);

        // lambert
        lambert += max(toSource.dot(normal), 0.0) * scalingFactor;

        // phong
        phong += pow(max(reflectedRay.dot(toSource), 0.0), shininess) * scalingFactor;
    }

    // apply splot lights
    for(int i=0; i<spotLights.size(); i++){
        // V1 = determine vector SP 
        // V1.normalize() 
        // V2 = direction of the source S 
        // V2.normalize() 
        // angle = acos(v1.dot(v2)) 
        // if angle > S.cutoff: 
        // P is not illuminated

        Vector3D sp = p - spotLights[i]->position; // vector from source to point
        sp.normalize();

        double angle = acos(-sp.dot(spotLights[i]->direction));
        if(RAD2DEG(angle) > spotLights[i]->cutoffangle){
            continue;
        }


        // check if the light source illuminates the point
        // make a ray from light to the point
        Ray lightRay(spotLights[i]->position, sp);
        double tMin = INF;
        Object* nearestObject = NULL;
        getNearestIntersectionPoint(lightRay, tMin, nearestObject);

        if(nearestObject != NULL && nearestObject != this){
            continue;
        }


        // vector to source
        Vector3D toSource = spotLights[i]->position - p;
        toSource.normalize();

        // distance between intersecting point and source
        double distance = (spotLights[i]->position - p).magnitude();

        // scaling factor
        double scalingFactor = exp(-distance*distance*spotLights[i]->falloff);

        // lambert
        lambert += max(toSource.dot(normal), 0.0) * scalingFactor;

        // phong
        phong += pow(max(reflectedRay.dot(toSource), 0.0), shininess) * scalingFactor;
    }

    // diffuse component
    c = c + getColor(p) * diffuse * max(lambert, 0.0);

    // specular component
    c = c + Color(1,1,1) * specular * max(phong, 0.0);

    // recursive reflection

    // check if recursive reflection is needed
    if(level==0){
        return;
    }

    // reflected ray
    Ray reflected(p, reflectedRay);

    // find the nearest object
    double tMin = INF;
    Object* nearestObject = NULL;
    getNearestIntersectionPoint(reflected, tMin, nearestObject);

    // if no object is intersected, return
    if(nearestObject == NULL){
        return;
    }

    // find the intersection point
    Color refelectedColor(0, 0, 0);

    // illuminate the intersection point
    nearestObject->intersectAndIlluminate(reflected, refelectedColor, level-1);

    // apply reflection
    c = c + refelectedColor * reflection;

    // clip color vlaues
    c.r = max(min(c.r, 1.0), 0.0);
    c.g = max(min(c.g, 1.0), 0.0);
    c.b = max(min(c.b, 1.0), 0.0);
}
