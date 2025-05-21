#ifndef FOG_H
#define FOG_H

#include <GL/gl.h>
#include "utils.h"

/**
 * Initialize the fog.
 */
void init_fog();

/**
 * Set the fog density.
 */
void set_fog_density(GLfloat density);

/**
 * Set the fog color.
 */
void set_fog_color(vec3 color);

#endif /* FOG_H */