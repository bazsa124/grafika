#include "cuboid.h"
#include <stdio.h>

int main(int argc, char* argv[]){
    Cuboid cube;
    double num1, num2, num3;

    printf("Enter three double values separated by spaces: ");
    scanf("%lf %lf %lf", &num1, &num2, &num3);

    set_square_data(&cube, num1,num2,num3);
    double area=calc_area(&cube);

    printf("Cuboid data: %.2f x %.2f x %.2f, area: %.2f",num1,num2,num3,area);

    return 0;
}