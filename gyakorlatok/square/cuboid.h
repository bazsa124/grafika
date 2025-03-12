#ifndef CUBOID_H
#define CUBOID_H

/**
 * Data of a circle object in Descartes coordinate system
 */
typedef struct
{
	double x;
	double y;
	double z;

} Cuboid;

/**
 * Set the data of the circle
 */
void set_square_data(Cuboid* cube, double x, double y, double z);

/**
 * Calculate the area of the circle.
 */
double calc_area(const Cuboid* cube);

#endif // CIRCLE_H