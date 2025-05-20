#include "utils.h"
#include <math.h>


double degree_to_radian(double degree)
{
	return degree * M_PI / 180.0;
}

double radian_to_degree(double radian)
{
	return radian * 180.0 / M_PI;
}

float min(float a, float b)
{
	return a < b ? a : b;
}

float max(float a, float b)
{
	return a > b ? a : b;
}

float clamp(float value, float mi, float ma)
{
	return min(ma, max(mi, value));
}