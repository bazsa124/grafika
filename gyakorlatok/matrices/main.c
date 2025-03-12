#include "matrix.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	float a[3][3] = {
        { 1.0f, -2.0f,  3.0f},
        { 5.0f, -3.0f,  0.0f},
        {-2.0f,  1.0f, -4.0f}
    };
	float b[3][3];
	float c[3][3];
    float i[3][3];

    init_zero_matrix(b); 
    b[1][1] =  8.0f;
    b[2][0] = -3.0f;
    b[2][2] =  5.0f;

    printf("\nA:\n");
    print_matrix(a);
    // printf("B:\n");
    // print_matrix(b);

    // printf("Szum:\n");
    // add_matrices(a, b, c);

    // print_matrix(c);
    // printf("Identify:\n");
    // init_identity_matrix(i);
    // scale_matrix(i,3);
    // print_matrix(i);

    // printf("Multiply:\n");
    // multiply_matrices(a,b,c);
    // print_matrix(c);

    float vector[2]={2.0f,3.0f};
    transform_point(a,vector);

    printf("\nResult:\n");
    print_matrix(a);
    
	return 0;
}

