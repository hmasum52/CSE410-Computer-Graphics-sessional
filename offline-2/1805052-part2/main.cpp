/**
 * @file main.cpp(stage1, stage2, stage3)
 * @author Hasan Masum (1805052@ugrad.cse.buet.ac.bd)
 * @brief 
 * @version 0.1
 * 
 * 
 */
#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include "matrix.h"
#include "vector3d.h"
#include "bitmap_image.hpp"
using namespace std;

// macro for setting width and precision
#define WIDTH_PRECISION(x, y) fixed<<setprecision(y)
// double infinity
#define INF numeric_limits<double>::infinity() 


// open file function with error check
// can open file in both read and write mode
// return a fstream object
fstream open_file(string filename, ios_base::openmode mode = ios_base::in){
    fstream file;
    file.open(filename, mode);
    if(!file.is_open()){
        cout<<"Error opening file: "<<filename<<endl;
        exit(1);
    }
    return file;
}

void writeTraingleVertex(ostream &os, const Matrix triangle){
    // print the first three elements of the matrix
    // also normalize the points
    for(int i = 0;i < 3;i++){
        for(int j = 0;j < 3;j++){
            double val = triangle.cell[j][i]/triangle.cell[3][i];
            os<<WIDTH_PRECISION(10, 7)<<val;
            if(j != 2) os<<" ";
        }
        os<<endl;
    }
    os<<endl;
}

// In the view transformation phase, the gluLookAt 
// parameters in scene.txt is used to generate the 
// view transformation matrix V, and the points in stage1.txt 
// are transformed by V and written in  stage2.txt. 
// generate view matrix V
// e: eye vector
// l: look vector
// u: up vector
Matrix generateViewMatrix(Vector3D eye, Vector3D look, Vector3D up){
    Vector3D l = (look - eye); l.normalize();
    Vector3D r = l.cross(up); r.normalize();
    Vector3D u = r.cross(l); 

    // Apply the following translation T to move the eye/camera to origin
    Matrix T = Matrix::identity();
    T.cell[0][3] = -eye.x; T.cell[1][3] = -eye.y; T.cell[2][3] = -eye.z;

    // Apply the following rotation R to align the camera with the world axes
    // that is aligning l with the -Z axis, r with X axis, and u with Y axis.
    Matrix R = Matrix::identity();
    R.cell[0][0] = +r.x; R.cell[0][1] = +r.y; R.cell[0][2] = +r.z;
    R.cell[1][0] = +u.x; R.cell[1][1] = +u.y; R.cell[1][2] = +u.z;
    R.cell[2][0] = -l.x; R.cell[2][1] = -l.y; R.cell[2][2] = -l.z;

    // view transformation matrix V = R * T
    return R * T;
}

//  The process of generating Projection matrix P
Matrix generateProjectionMatrix(double fovY, double asepectRatio, double rear, double far){
    // First compute the field of view along X (fovX) axis
    double fovX = fovY * asepectRatio;

    // then determine r and t
    double t = rear * tan((fovY/2) * (M_PI/180));
    double r = rear * tan((fovX/2) * (M_PI/180));

    // make the projection matrix P
    Matrix P; // zero matrix
    P.cell[0][0] = rear/r; P.cell[1][1] = rear/t; P.cell[2][2] = -(far + rear)/(far - rear);
    P.cell[2][3] = -(2*far*rear)/(far - rear); P.cell[3][2] = -1;
    // matrix look like this
    // [rear/r 0                    0                          0            ]
    // [0      rear/t               0                          0            ]
    // [0      0       -(far + rear)/(far - rear) -(2*far*rear)/(far - rear)]
    // [0      0                   -1                          0            ]

    return P;
}

class Traingle {
    public:
    // array of vertex
    Vector3D vertex[3];

    // color of each vertex
    int color[3];
};

static unsigned long int g_seed = 1;
inline int random()
{
    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

int main(int argc, char* argv[]){
    //string filename = argc > 1 ? argv[1] : "scene.txt";
    string filename = "../test-cases-part2/1/scene.txt";
    fstream scene = open_file(filename, ios_base::in);

    // Lines 1-3 of scene.txt state the parameters
    // of the gluLookAtfunction
    Vector3D eye, look, up;
    scene >> eye >> look >> up;

    //cout<<"eye: "<<eye<<", "<<"look: "<<look<<", "<<"up: "<<up<<endl;
    
    //Line 4 provides the gluPerspective parameters
    double fovY, aspectRatio, near, far;
    scene >> fovY >> aspectRatio >> near >> far;  

    /* cout << "fovY: " << fovY << ", "
            << "aspectRatio: " << aspectRatio << ", "
            << "near: " << near << ", "
            << "far: " << far << std::endl; */

    // open output files
    fstream stage1 = open_file("stage1.txt", ios_base::out);
    fstream stage2 = open_file("stage2.txt", ios_base::out);
    fstream stage3 = open_file("stage3.txt", ios_base::out);


    // initialize empty stack S
    stack<Matrix> S; 
    // initialize identity matrix M
    Matrix M = Matrix::identity();

    // Stage 2: View Transformation
    // In the view transformation phase,
    // the gluLookAt parameters in scene.txt is used 
    // to generate the  view transformation matrix V
    Matrix V = generateViewMatrix(eye, look, up);

    
    // Stage 3: Projection Transformation
    // In the projection transformation phase, 
    // the gluPerspective parameters in scene.txt are used to 
    // generate the projection transformation matrix P
    Matrix P = generateProjectionMatrix(fovY, aspectRatio, near, far);

    // for stage4
    vector<Traingle> triangles;


    string command;
    while(true){
        // read command
        scene >> command;
        // cout<<command<<endl;

        // command switch statement
        if(command == "triangle"){
            //cout<<"triangle"<<endl;

            // pseudocode:
            // for each three points
            //      P’ <- transformPoint(M, P)
            //      output P’

            // instead of looping we make a 4*4 traingle matrix
            // and apply the transformation matrix to it
            // then we print the first three elements of the matrix
            // which are the transformed points
            Matrix triangle(1); // all cell values are 1

            // read three points
            scene >> triangle.cell[0][0] >> triangle.cell[1][0] >> triangle.cell[2][0];
            scene >> triangle.cell[0][1] >> triangle.cell[1][1] >> triangle.cell[2][1];
            scene >> triangle.cell[0][2] >> triangle.cell[1][2] >> triangle.cell[2][2];
            // traingle matrix now look like this
            // [x1 x2 x3 1]
            // [y1 y2 y3 1]
            // [z1 z2 z3 1]
            // [ 1  1  1 1]

            // =================== stage1 =====================
            // ============= Model Transformation =============
            // ================================================

            // apply transformation matrix
            triangle = M * triangle;
            // save the coordinates in stage.txt file
            writeTraingleVertex(stage1, triangle);

            // for example if we apply translation matrix T
            // then the traingle matrix will look like this
            // [1 0 0 Tx]   [x1 x2 x3 1]   [x1+Tx x2+Tx x3+Tx 1]
            // [0 1 0 Ty]   [y1 y2 y3 1]   [y1+Ty y2+Ty y3+Ty 1]
            // [0 0 1 Tz] * [z1 z2 z3 1]   [z1+Tz z2+Tz z3+Tz 1]
            // [0 0 0  1]   [ 1  1  1 1]   [   1    1    1    1]

            // =================== stage2 =====================
            // ============= View Transformation ==============
            // ================================================

            // apply view transformation matrix
            triangle = V*triangle;
            // save the coordinates in stage2.txt file
            writeTraingleVertex(stage2, triangle);

            // =================== stage3 =====================
            // ============= Projection Transformation ========
            // ================================================

            // apply projection transformation matrix
            triangle = P*triangle;
            // save the coordinates in stage3.txt file
            writeTraingleVertex(stage3, triangle);

            // save traingle in triangles vector for stage4
            Traingle t;
            // also normalize the coordinates
            // use for loop
            for (int i = 0; i < 3; i++){
                t.vertex[i] = Vector3D(triangle.cell[0][i]/triangle.cell[3][i],
                                        triangle.cell[1][i]/triangle.cell[3][i],
                                        triangle.cell[2][i]/triangle.cell[3][i]);
                // assign a random color in 0 to 255 range
                t.color[i] = random() % 256;
            }
            triangles.push_back(t);
        }
        else if(command == "translate"){
            //cout<<"translate"<<endl;
            // read translation amounts and
            // generate the corresponding translation matrix T
            Matrix T = Matrix::identity();
            scene>>T.cell[0][3]>>T.cell[1][3]>>T.cell[2][3];

            // M = product(M, T)
            M = M * T;
        }
        else if(command == "scale"){
            // cout<<"scale"<<endl;

            // read scaling factors and
            // generate the corresponding scaling matrix S
            Matrix S = Matrix::identity();
            scene>>S.cell[0][0]>>S.cell[1][1]>>S.cell[2][2];

            // M = product(M, S)
            M = M * S;
        }
        else if(command == "rotate"){
            // cout<<"rotate"<<endl;
            // read rotation angle and axis
            // generate the corresponding rotation matrix R
            double angle; 
            Vector3D axis;
            scene>>angle>>axis;
            
            // angle is in degree
            angle = angle * M_PI / 180.0;

            // columns of the rotation matrix indicate where the
            // unit vectors along the principal axes(namely i,j,k) are
            // after rotation
            Vector3D i(1, 0, 0), j(0, 1, 0), k(0, 0, 1);

            // we will use the vector form of Rodrigues' rotation formula
            // to determine where i, j, k are transformed and
            axis.normalize();
            Vector3D c1 = applyRodriguesFormula(i, axis, angle);
            Vector3D c2 = applyRodriguesFormula(j, axis, angle);
            Vector3D c3 = applyRodriguesFormula(k, axis, angle);

            // then use those to generate the rotation matrix
            Matrix R = Matrix::identity();
            R.cell[0][0] = c1.x; R.cell[0][1] = c2.x; R.cell[0][2] = c3.x;
            R.cell[1][0] = c1.y; R.cell[1][1] = c2.y; R.cell[1][2] = c3.y;
            R.cell[2][0] = c1.z; R.cell[2][1] = c2.z; R.cell[2][2] = c3.z;

            // M = product(M, R)
            M = M * R;
        }
        else if(command == "push"){
            // cout<<"push"<<endl;
            S.push(M);
        }
        else if(command == "pop"){
            // cout<<"pop"<<endl;
            M = S.top();
            S.pop();
        }
        else if(command == "end"){
            // cout<<"end"<<endl;
            break;
        }
        // else{
        //     cout<<"Invalid command: "<<command<<endl;
        //     exit(1);
        // }

    }


    stage1.close();
    stage2.close();
    stage3.close();
    // ======================================================================================
    // ============= Stage 4: Clipping & scan conversion using Z-buffer algorithm ===========
    // ======================================================================================

    // 1. READ DATA

    // open config file
    string config_filename = argc > 2 ? argv[2] : "../test-cases-part2/1/config.txt";
    fstream config = open_file(config_filename, ios::in);

    // take screen width and height as input
    int screen_width, screen_height;
    config>>screen_width>>screen_height;

    // debug 
    // print screen width and height
    // cout<<"screen width: "<<screen_width<<endl;
    // cout<<"screen height: "<<screen_height<<endl<<endl;

    // // print triangles
    // for(auto triangle: triangles){
    //     cout<<triangle.vertex[0]<<endl;
    //     cout<<triangle.vertex[1]<<endl;
    //     cout<<triangle.vertex[2]<<endl;
    //     cout<<endl;
    // }

    // 2. Initialize z-buffer and frame buffer

    // 2.a Create a pixel mapping between the x-y range values 
    // and the Screen_Width X Screen_height range.
    int x_left_limit = -1;
    int x_right_limit = 1;
    int y_bottom_limit = -1;
    int y_top_limit = 1;

    double dx = (x_right_limit - x_left_limit) / (double)screen_width;
    double dy = (y_top_limit - y_bottom_limit) / (double)screen_height;

    // besides specify Top_Y and Left_X values.
    // also specify Bottom_Y and Right_X values.
    double top_y = y_top_limit - dy/2.0;
    double left_x = x_left_limit + dx/2.0;
    double bottom_y = y_bottom_limit + dy/2.0;
    double right_x = x_right_limit - dx/2.0;

    // debug
    // cout<<"dx: "<<dx<<endl;
    // cout<<"dy: "<<dy<<endl;
    // cout<<"top_y: "<<top_y<<endl;
    // cout<<"left_x: "<<left_x<<endl;
    // cout<<"bottom_y: "<<bottom_y<<endl;
    // cout<<"right_x: "<<right_x<<endl<<endl;

    // 2.c Create a z-buffer, a two dimensional array of Screen_Width X Screen_Height dimension. 
    // Initialize all the values in z-buffer with z_max. In the aforementioned examples, z_max = 2.0. 
    // The memory for z-buffer should be dynamically allocated (using STL is allowed).
    double** z_buffer = new double*[screen_width];

    int z_min = -1;
    int z_max = 1;
    for(int i = 0; i < screen_width; i++){
        z_buffer[i] = new double[screen_height];
        for(int j = 0; j < screen_height; j++){
            z_buffer[i][j] = z_max;
        }
    }

    // 2.d Create a bitmap_image object with Screen_Width X Screen_Height resolution and 
    // initialize its background color with black
    //cout<<"Creating image..."<<endl;
    bitmap_image image(screen_width, screen_height);

    // 3. Apply procedure
    // a. Pseudocode:

    int cnt = 0;
    for(auto t: triangles){
        cnt++;
        // clipping
        // Compute the max_y for each triangle. If max_y > Top_Y, clip (i.e. ignore) the portion 
        // above Top_Y and start scanning from Top_Y. Otherwise find the suitable mapping of 
        // max_y to a top_scanline below Top_Y. Do a similar checking for min_y and Bottom_Y.

        // min y value traingle vertex
        double min_y = min(t.vertex[0].y, min(t.vertex[1].y, t.vertex[2].y));
        // max y value traingle vertex
        double max_y = max(t.vertex[0].y, max(t.vertex[1].y, t.vertex[2].y));

        cout<<"triangle "<<cnt-1<<endl;
        // cout<<"min_y: "<<min_y<<endl;
        // cout<<"max_y: "<<max_y<<endl;
        // cout<<endl; //ok
        

        // Find top_scanline and bottom_scanline after necessary clipping
        double top_scanline = round( (top_y - min(top_y, max_y)) / dy );
        double bottom_scanline = (screen_height -1 ) - round( (max(bottom_y, min_y) - bottom_y) / dy );

        // cout<<"top_scanline: "<<top_scanline<<endl;
        // cout<<"bottom_scanline: "<<bottom_scanline<<endl;
        // cout<<endl; // ok

        for(int row_no = top_scanline; row_no <= bottom_scanline; row_no++){
            // During scanning from top to bottom and left to right, check for the middle values of each cell. 
            // e.g. Top_Y- row_no*dy, Left_X+col_no*dx


            // Compute min_x for each scan line for each triangle. If min_x < Left_X, clip (i.e. ignore) 
            // the portion to the left of Left_X. Otherwise find the suitable mapping of min_x to a 
            // column to the right of Left_X (left_intersecting_column). Do a similar checking for max_x 
            // and Right_X
            
            // find the intersecting point of the scanline with the triangle
            // for all triangle line

            double scanline_y = top_y - row_no*dy; // y coordinate of the scanline
            
            Vector3D point1; // left intersecting point
            Vector3D point2; // right intersecting point
            // init with inf
            point1.x = INF; // to check if the point is initialized
            point2.x = - INF; // default value check if the point is intersecting with the triangle

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

                    // update point1 and point2
                    
                    // check if v1 and v2 has the same y value that is the line is horizontal
                    if(v1.y == v2.y){
                        // now if v1 and scanline has the same y value 
                        // then v1 might be an intersecting point
                        if(v1.y == scanline_y){
                            // if v1 is an intersecting point then
                            // check if v1 is the leftmost or rightmost point
                            if(v1.x < point1.x) point1 = v1; // if v1 is the leftmost point
                            if(v1.x > point2.x) point2 = v1; // if v1 is the rightmost point

                            // if v1 is not the intersecting point then 
                            // check if v2 is the intersecting point
                            if(v2.x < point1.x) point1 = v2; // if v2 is the leftmost point
                            if(v2.x > point2.x) point2 = v2; // if v2 is the rightmost point
                        }

                    }else { // the line is not horizontal
                        if (x < point1.x) point1 = Vector3D(x, scanline_y, z); // if x is the leftmost point x
                        if (x > point2.x) point2 = Vector3D(x, scanline_y, z); // if x is the rightmost point x
                    }
                }
            }

            // if the point is not initialized then there is no intersecting point
            if (point1.x == INF or point2.x == - INF) continue;

            cout<<"point1: "<<point1<<endl;
            cout<<"point2: "<<point2<<endl;

            // Find left_intersecting_column and right_intersecting_column
            // after necessary clipping.
            int left_intersecting_column = (int)round( (max(left_x, point1.x) - left_x) / dx );
            int right_intersecting_column = (screen_width -1 ) - (int)round( (right_x - min(right_x, point2.x)) / dx );
            cout<<"scanline_y: "<<scanline_y<<endl;
            cout<<"left_intersecting_column: "<<left_intersecting_column<<endl;
            cout<<"right_intersecting_column: "<<right_intersecting_column<<endl;

            // for col_no from left_intersecting_column to right_intersecting_column
            for (int col_no=left_intersecting_column; col_no <=right_intersecting_column; col_no++){

                // Calculate z value for the current column
                double scanline_x = left_x + col_no*dx; // x coordinate of the scanline
                double z = (scanline_x - point1.x) * (point2.z - point1.z) / (point2.x - point1.x) + point1.z;



                // Check if object is within z-range of vision
                if (z >= z_min && z <= z_max) {
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

    // open zBufferFile
    fstream zBufferFile = open_file("z-buffer.txt", ios::out);

        // Output zBuffer
    for (int i=0; i < screen_height; i++) {
        for (int j=0; j < screen_width; j++) {
        if (z_buffer[i][j] < z_max)
            zBufferFile << z_buffer[i][j] << " ";
        }
        zBufferFile << endl;
    }

    // Delete zBuffer
    for(int i = 0; i < screen_width; ++i) {
        delete[] z_buffer[i];   
    }
    delete[] z_buffer;

    image.save_image("out.bmp");
}