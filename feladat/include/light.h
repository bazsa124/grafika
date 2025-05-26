#ifndef LIGHT_H
#define LIGHT_H

#include "object.h"

typedef struct Light Light;

struct Light
{
    int id;

    vec3 position;
    vec3 real_position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float strength;

    float constant;
    float linear;
    float quadratic;

    Thing* parent;
};

/**
 * Allocate a new light.
 */
Light* new_light();

/**
 * Initialize the light.
 */
void init_light(Light* light, int id);

/**
 * Set the light's position.
 */
void set_light_position(Light* light, vec3 position);

/**
 * Set the light's base color. 
 */
void set_light_color(Light* light, vec3 ambient, vec3 diffuse, vec3 specular);

/**
 * Set the light's attenuation.
 */
void set_light_attenuation(Light* light, float constant, float linear, float quadratic);

/**
 * Set the light's parent.
 */
void set_light_parent(Light* light, Thing* parent);

/**
 * Set light properties.
 */
void set_light(Light* light);

#endif /* LIGHT_H */