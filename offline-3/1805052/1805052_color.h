#pragma once
#include <iostream>
using namespace std;

class Color{
    public:
    double r;
    double g;
    double b;

    Color(){
        r = g = b = 0;
    }

    Color(double r, double g, double b){
        this->r = r;this->g = g;this->b = b;
    }

    friend ostream& operator<<(ostream& os, const Color& c){
        os<<"("<<c.r<<","<<c.g<<","<<c.b<<")";
        return os;
    }

    // frend function for input
    friend istream& operator>>(istream& is, Color& c){
        is>>c.r>>c.g>>c.b;
        return is;
    }
};