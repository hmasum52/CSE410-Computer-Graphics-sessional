#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include "matrix.h"
#include "vector3d.h"
#include "stack.h"
using namespace std;

// macro for setting width and precision
#define WIDTH_PRECISION(x, y) fixed<<setprecision(y)


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



int main(int argc, char* argv[]){
    string filename = argc > 1 ? argv[1] : "scene.txt";
    //string filename = "../test-cases/3/scene.txt";
    fstream scene = open_file(filename, ios_base::in);

    // Lines 1-3 of scene.txt state the parameters
    // of the gluLookAtfunction
    Vector3D eye, look, up;
    scene >> eye >> look >> up;

    cout<<"eye: "<<eye<<endl;
    cout<<"look: "<<look<<endl;
    cout<<"up: "<<up<<endl;
    
    //Line 4 provides the gluPerspective parameters
    double fovY, aspectRatio, near, far;
    scene >> fovY >> aspectRatio >> near >> far;  

    cout<<"fovY: "<<fovY<<endl;
    cout<<"aspectRatio: "<<aspectRatio<<endl;
    cout<<"near: "<<near<<endl;
    cout<<"far: "<<far<<endl;

    // open output files
    fstream stage1 = open_file("stage1.txt", ios_base::out);
    fstream stage2 = open_file("stage2.txt", ios_base::out);
    fstream stage3 = open_file("stage3.txt", ios_base::out);


    // initialize empty stack S
    stack<Matrix> S; 
    // initialize identity matrix M
    Matrix M = Matrix::identity();

    // read from scence file
    cout<<"reading commands"<<endl;
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

            // apply transformation matrix
            triangle = M * triangle;

            // save the coordinates in stage.txt file
            writeTraingleVertex(stage1, triangle);            

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
}