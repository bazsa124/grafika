#include "cuboid.h"

#include <math.h>

void set_square_data(Cuboid* square, double x, double y, double z)
{

	if (x >0.0) {
        square->x=x;
	} else {
		square->x = NAN;
	}

    if (y >0.0) {
        square->y=y;
	} else {
		square->y = NAN;
	}

    if (z >0.0) {
        square->z=z;
	} else {
		square->z = NAN;
	}
}

double calc_area(const Cuboid* square)
{
	double area = square->x * square->y *square->z;
	return area;
}
