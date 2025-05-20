#ifndef UTILS_H
#define UTILS_H

#include <math.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
/**
 * GLSL-like three dimensional vector
 */
typedef struct vec3
{
    float x;
    float y;
    float z;
} vec3;

#define new_vec3(x, y, z)    ((vec3){x, y, z})
#define set_vec3(v, x, y, z) v.x = x; v.y = y; v.z = z

#define vec3_zero() new_vec3(0, 0, 0)
#define vec3_one()  new_vec3(1, 1, 1)

#define add_vec3(v1, v2) new_vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z)
#define sub_vec3(v1, v2) new_vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z)
#define mul_vec3(v, s)   new_vec3(v.x * s, v.y * s, v.z * s)
#define div_vec3(v, s)   new_vec3(v.x / s, v.y / s, v.z / s)

#define len_vec3(v)      sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
#define norm_vec3(v)     div_vec3(v, len_vec3(v))

/**
 * Color with RGB components
 */
typedef struct Color
{
    float red;
    float green;
    float blue;
} Color;

/**
 * Material
 */
typedef struct Material
{
    struct vec3 ambient;
    struct vec3 diffuse;
    struct vec3 specular;
    struct vec3 emission;
    float shininess;
} Material;

/**
 * Calculates radian from degree.
 */
double degree_to_radian(double degree);

/**
 * Calculates degree from radian.
 */
double radian_to_degree(double radian);

/**
 * Returns the minimum of two values.
 */
float min(float a, float b);

/**
 * Returns the maximum of two values.
 */
float max(float a, float b);

/**
 * Clamps the value between min and max.
 */
float clamp(float value, float mi, float ma);

#endif /* UTILS_H */
