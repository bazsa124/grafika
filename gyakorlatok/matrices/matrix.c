#include "matrix.h"
#include <math.h>
#include <stdio.h>

void init_zero_matrix(float matrix[3][3])
{
    int i;
    int j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            matrix[i][j] = 0.0;
        }
    }
}

void print_matrix(const float matrix[3][3])
{
    int i;
    int j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            printf("%4.4f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void add_matrices(const float a[3][3], const float b[3][3], float c[3][3])
{
    int i;
    int j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

//Generate Identity Matrix
void init_identity_matrix(float matrix[3][3]){
    int i;
    int j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            if(i==j)
                matrix[i][j]=1.0;
            else
                matrix[i][j] = 0.0;
        }
    }
}

//Multiply whole Matrix by a Skalar
void scale_matrix(float matrix[3][3], double skalar){
    int i;
    int j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {

                matrix[i][j]*=skalar;

        }
    }
}

//Multiply Matrix by Matrix
void multiply_matrices(float a[3][3], float b[3][3], float result[3][3]){
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i][j]=0.0;
            for( int x=0; x<3; x++){
                result[i][j]+=b[x][j]*a[i][x];
            }
        }
    }
}

//Transform by 2D vector
void transform_point(float matrix[3][3], float vector2[2]){
    float t_matrix[3][3];
    float result[3][3];

    init_identity_matrix(t_matrix);

    t_matrix[0][2]=vector2[0];
    t_matrix[1][2]=vector2[1];

    multiply_matrices(matrix,t_matrix,result);

    copy_matrix(matrix,result);

}

//Scale Matric in 2D
void scaling(float vector[2],float a[3][3]){
    float t_matrix[3][3];
    init_identity_matrix(t_matrix);

    t_matrix[0][0]=vector[0];
    t_matrix[1][1]=vector[1];

    float result[3][3];

    multiply_matrices(a,t_matrix,result);
    copy_matrix(a,result);
}

void copy_matrix(float to[3][3], float from[3][3]){
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            to[i][j]=from[i][j];
        }
    }
}

void rotate_matrix(float degree, float matrix[3][3]){
    float t_matrix[3][3];

    init_identity_matrix(t_matrix);

    t_matrix[0][0]=cos(degree);
    t_matrix[1][1]=cos(degree);

    t_matrix[0][1]=-sin(degree);
    t_matrix[1][0]=sin(degree);

    float result[3][3];
    multiply_matrices(matrix,t_matrix,result);

    copy_matrix(matrix,result);
}