#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include "matrix.h"
#include "vector3d.h"
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



int main(int argc, char* argv[]){
    string filename = argc > 1 ? argv[1] : "scene.txt";
    //string filename = "../test-cases/3/scene.txt";
    fstream scene = open_file(filename, ios_base::in);

    // Lines 1-3 of scene.txt state the parameters
    // of the gluLookAtfunction
    Vector3D eye, look, up;
    scene >> eye >> look >> up;

    cout<<"eye: "<<eye<<", "<<"look: "<<look<<", "<<"up: "<<up<<endl;
    
    //Line 4 provides the gluPerspective parameters
    double fovY, aspectRatio, near, far;
    scene >> fovY >> aspectRatio >> near >> far;  

    cout << "fovY: " << fovY << ", "
            << "aspectRatio: " << aspectRatio << ", "
            << "near: " << near << ", "
            << "far: " << far << std::endl;

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