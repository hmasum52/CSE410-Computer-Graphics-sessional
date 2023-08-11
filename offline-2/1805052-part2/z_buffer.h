#pragma once
#include <bits/stdc++.h>
#include "vector3d.h"
#include "bitmap_image.hpp"
using namespace std;
// double infinity
#define INF INT_MAX
#define max3(a, b, c) max(a, max(b, c))
#define min3(a, b, c) min(a, min(b, c))
class Color{
    public:
    int r;
    int g;
    int b;

    Color(){
        r = g = b = 0;
    }

    Color(int r, int g, int b){
        this->r = r;this->g = g;this->b = b;
    }
};


class Traingle {
    public:
    // array of vertex
    Vector3D vertex[3];

    // color of each vertex
    int color[3];
};


// class to apply z-buffer algorithm
class ZBuffer{
    vector<Traingle> triangles;
    int screen_width;
    int screen_height;

    // defined in constructor
    // 2.a Create a pixel mapping between the x-y range values 
    // and the Screen_Width X Screen_height range.
    int x_left_limit;
    int x_right_limit;
    int y_bottom_limit;
    int y_top_limit;

    double dx;
    double dy;

    double top_y;
    double bottom_y;
    double left_x;
    double right_x;

    double** z_buffer;

    int z_min;
    int z_max;

    bitmap_image image;
public:
    // constructor
    ZBuffer(vector<Traingle> triangles, int screen_width, int screen_height){
        this->triangles = triangles;
        this->screen_width = screen_width;
        this->screen_height = screen_height;

        // 2.a Create a pixel mapping between the x-y range values
        // and the Screen_Width X Screen_height range.
        x_left_limit = -1;
        x_right_limit = 1;
        y_bottom_limit = -1;
        y_top_limit = 1;
        dx = (x_right_limit - x_left_limit) / (double)screen_width;
        dy = (y_top_limit - y_bottom_limit) / (double)screen_height;


        // normalized coordinate of the top-left corner and bottom-right corner of the screen
        left_x = x_left_limit + dx/2.0;
        right_x = x_right_limit - dx/2.0;
        bottom_y = y_bottom_limit + dy/2.0;
        top_y = y_top_limit - dy/2.0;

        // 2.c Create a z-buffer, a two dimensional array of Screen_Width X Screen_Height dimension. 
        // Initialize all the values in z-buffer with z_max. In the aforementioned examples, z_max = 2.0. 
        // The memory for z-buffer should be dynamically allocated (using STL is allowed).
        z_buffer = new double*[screen_width];

        // init z buffer
        z_min = -1;
        z_max = 1;
        for(int i = 0; i < screen_width; i++){
            z_buffer[i] = new double[screen_height];
            for(int j = 0; j < screen_height; j++){
                z_buffer[i][j] = z_max;
            }
        }

        // 2.d Create a bitmap_image object with Screen_Width X Screen_Height resolution and 
        // initialize its background color with black
        //cout<<"Creating image..."<<endl;
        image.setwidth_height(screen_width, screen_height);
    }

    // destructor
    ~ZBuffer(){
        for(int i = 0; i < screen_width; i++){
            delete[] z_buffer[i];
        }
        delete[] z_buffer;
    }

    // apply z buffer algorithm
    void apply(){
        int cnt = 0;
        for(auto t: triangles){
            // cout<<"triangle "<<cnt<<endl;
            cnt++;
            // clipping 1

            // Compute the max_y for each triangle. 
            double max_y = max3(t.vertex[0].y, t.vertex[1].y, t.vertex[2].y);
            // If max_y > Top_Y, clip (i.e. ignore) the portion above Top_Y and start scanning from Top_Y.
            // Otherwise find the suitable mapping of max_y to a top_scanline below Top_Y.
            int top_scanline = (int)round( (top_y - min(top_y, max_y)) / dy );

            //  Do a similar checking for min_y and Bottom_Y.
            double min_y = min3(t.vertex[0].y, t.vertex[1].y, t.vertex[2].y);
            int bottom_scanline = screen_height - (int)round( (max(bottom_y, min_y) - bottom_y) / dy );

            // debug
            // cout<<"min_y: "<<min_y<<endl;
            // cout<<"max_y: "<<max_y<<endl;
            // cout<<"top_scanline: "<<top_scanline<<endl;
            // cout<<"bottom_scanline: "<<bottom_scanline<<endl;
            // cout<<endl; // ok

            // scan from top to bottom
            for(int row_no = top_scanline; row_no <= bottom_scanline; row_no++){

                // During scanning from top to bottom, check for the middle values of each cell. 
                // i.e. Top_Y- row_no*dy
                double scanline_y = top_y - row_no*dy; // y coordinate of the scanline
                
                // find the intersecting point of the scanline with the triangle
                // for all triangle line

                // clipping 2

                // Compute min_x and max_x for each scan line for each triangle. 
                double min_x;
                double max_x;
                double start_z;
                double end_z;
                
                // init with inf
                min_x = INF;
                max_x = -INF;

                for (int i=0; i<3; i++) {
                    // get current and next vertex
                    Vector3D v1 = t.vertex[i]; // current vertex
                    Vector3D v2 = t.vertex[(i+1)%3]; // next vertex

                    // potiential intersecting point x
                    double x = (scanline_y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y) + v1.x;

                    // check if the intersecting point is within the triangle
                    if ( x >= min(v1.x, v2.x) and x <= max(v1.x, v2.x) ) {
                        // base z
                        double z = (scanline_y - v1.y) * (v2.z - v1.z) / (v2.y - v1.y) + v1.z;

                        // update pLeft and pRight
                        
                        // check if v1 and v2 has the same y value that is the line is horizontal
                        if(v1.y == v2.y){
                            // now if v1 and scanline has the same y value 
                            // then v1 might be an intersecting point
                            if(v1.y == scanline_y){
                                // if v1 is an intersecting point then
                                // check if v1 is the leftmost or rightmost point
                                if(v1.x < min_x) min_x = v1.x, start_z = v1.z; // if v1 is the leftmost point
                                if(v1.x > max_x) max_x = v1.x, end_z = v1.z; // if v1 is the rightmost point

                                // if v1 is not the intersecting point then 
                                // check if v2 is the intersecting point
                                if(v2.x < min_x) min_x = v2.x, start_z = v2.z; // if v2 is the leftmost point
                                if(v2.x > max_x) max_x = v2.x, end_z = v2.z; // if v2 is the rightmost point
                            }

                        }else { // the line is not horizontal
                            if (x < min_x) min_x = x, start_z = z; // if x is the leftmost point x
                            if (x > max_x) max_x = x, end_z = z; // if x is the rightmost point x
                        }
                    }
                }

                // if the point is not initialized then there is no intersecting point
                if (min_x == INF or max_x == - INF) continue;

                //cout<<"pLeft: "<<min_x<<" "<<scanline_y<<" "<<start_z<<endl;
                //cout<<"pRight: "<<max_x<<" "<<scanline_y<<" "<<end_z<<endl;

                // If min_x < Left_X, clip (i.e. ignore) 
                // the portion to the left of Left_X. Otherwise find the suitable mapping of min_x to a 
                // column to the right of Left_X (left_intersecting_column). Do a similar checking for max_x 
                // and Right_X

                // Find left_intersecting_column and right_intersecting_column
                // after necessary clipping.
                int left_intersecting_column = (int)round( (max(left_x, min_x) - left_x) / dx );
                int right_intersecting_column = screen_width - (int)round( (right_x - min(right_x, max_x)) / dx );
                // cout<<"scanline_y: "<<scanline_y<<endl;
                // cout<<"left_intersecting_column: "<<left_intersecting_column<<endl;
                // cout<<"right_intersecting_column: "<<right_intersecting_column<<endl;

                // scan from left to right
                // for col_no from left_intersecting_column to right_intersecting_column
                for (int col_no=left_intersecting_column; col_no <=right_intersecting_column; col_no++){

                    // During scanning from left to right, check for the middle values of each cell. 
                    // e.g. Left_X+col_no*dx
                    double scanline_x = left_x + col_no*dx; // x coordinate of the scanline
                    // Calculate z value for the current column
                    double z = (scanline_x - min_x) * (end_z - start_z) / (max_x - min_x) + start_z;

                    // Check if object is within z-range of vision
                    if (z >= z_min and z <= z_max) {
                        // Check if current z value is closer than previous z value for this pixel
                        if (z < z_buffer[row_no][col_no]) {
                            // Update z-buffer and pixel color
                            z_buffer[row_no][col_no] = z;
                            image.set_pixel(col_no, row_no, t.color[0], t.color[1], t.color[2]);
                        }
                    }
                }
            }
        }
    }

    // save z-buffer to a file
    void save(string filename){
        ofstream file;
        file.open(filename);
        // output z buffer may not match with test output
        // but image will similar to test output
        for (int i=0; i < screen_height; i++) {
            for (int j=0; j < screen_width; j++) {
            if (z_buffer[i][j] < z_max)
                file << z_buffer[i][j] << "\t";
            }
            file << endl;
        }
        file.close();
    }

    // function to save the image
    void save_image(string filename){
        image.save_image(filename);
    }
};

