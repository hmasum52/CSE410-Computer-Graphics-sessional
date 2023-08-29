#pragma once
#include <fstream>
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