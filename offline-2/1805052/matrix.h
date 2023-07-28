/**
 * @file matrix.h
 * @author Hasan Masum (1805052@ugrad.cse.buet.ac.bd)
 * @brief 
 * @version 0.1
 * @date 2023-07-28
 * 
 */
#pragma once


#include <bits/stdc++.h>
using namespace std;

/**
 * @brief 4*4 matrix class
 * 
 */
class Matrix{
    public:
    double cell[4][4];

    /**
     * @brief Construct a new Zero Matrix object
     * 
     */
    Matrix(){
        for(int i = 0;i < 4;i++){
            for(int j = 0;j < 4;j++){
                cell[i][j] = 0;
            }
        }
    }

    // create a matrix with all cell values are n
    Matrix(int n){
        for(int i = 0;i < 4;i++){
            for(int j = 0;j < 4;j++){
                cell[i][j] = n;
            }
        }
    }

    // make identity matrix
    void makeIndentity(){
        for(int i = 0;i < 4;i++){
            for(int j = 0;j < 4;j++){
                cell[i][j] = i == j ? 1 : 0;
            }
        }
    }

    // static method to make indentiy matrix
    static Matrix identity(){
        Matrix m;
        m.makeIndentity();
        return m;
    }

    // friend function to overload << operator
    friend ostream& operator<<(ostream& os, const Matrix& m){
        for(int i = 0;i < 4;i++){
            for(int j = 0;j < 4;j++){
                os<<m.cell[i][j]<<" ";
            }
            os<<endl;
        }
        return os;
    }

    // muliply two matrix with *
    Matrix operator*(const Matrix& m){
        Matrix res;
        for(int i = 0;i < 4;i++){
            for(int j = 0;j < 4;j++){
                for(int k = 0;k < 4;k++){
                    res.cell[i][j] += cell[i][k] * m.cell[k][j];
                }
            }
        }
        return res;
    }
};