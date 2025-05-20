#include "light.h"

Light* new_light()
{
    Light* light = malloc(sizeof(Light));
    if (!light)
    {
        fprintf(stderr, "[ERROR] Failed to allocate memory for light.\n");
        exit(1);
    }

    return light;
}

void init_light(Light* light, int id)
{
    light->id = id;

    light->position      = vec3_zero();
    light->real_position = vec3_zero();

    light->ambient  = new_vec3(0.1f, 0.1f, 0.1f);
    light->diffuse  = new_vec3(1.0f, 1.0f, 1.0f);
    light->specular = new_vec3(1.0f, 1.0f, 1.0f);

    light->strength = 1.0f;

    light->constant  = 1.0f;
    light->linear    = 0.2f;
    light->quadratic = 0.08f;

    light->parent = NULL;

    glEnable(GL_LIGHT0 + light->id);
}

void set_light_position(Light* light, vec3 position)
{
    light->position = position;
}

void set_light_color(Light* light, vec3 ambient, vec3 diffuse, vec3 specular)
{
    light->ambient  = ambient;
    light->diffuse  = diffuse;
    light->specular = specular;
}

void set_light_attenuation(Light* light, float constant, float linear, float quadratic)
{
    light->constant  = constant;
    light->linear    = linear;
    light->quadratic = quadratic;
}

void set_light_parent(Light* light, Thing* parent)
{
    light->parent = parent;
}

// Objects for caching the light properties.
float ambient_light[4] ;
float diffuse_light[4] ;
float specular_light[4];
float position[4]      ;

void set_light(Light* light)
{
    ambient_light[0] = light->ambient.x * light->strength;
    ambient_light[1] = light->ambient.y * light->strength;
    ambient_light[2] = light->ambient.z * light->strength;
    ambient_light[3] = light->strength;

    diffuse_light[0] = light->diffuse.x * light->strength;
    diffuse_light[1] = light->diffuse.y * light->strength;
    diffuse_light[2] = light->diffuse.z * light->strength;
    diffuse_light[3] = light->strength;

    specular_light[0] = light->specular.x * light->strength;
    specular_light[1] = light->specular.y * light->strength;
    specular_light[2] = light->specular.z * light->strength;
    specular_light[3] = light->strength;

    if (light->parent)
        light->real_position = add_vec3(light->parent->position, light->position);
    else
        light->real_position = light->position;

    position[0] = light->real_position.x;
    position[1] = light->real_position.y;
    position[2] = light->real_position.z;
    position[3] = 1.0f;

    glLightfv(GL_LIGHT0 + light->id, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0 + light->id, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0 + light->id, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0 + light->id, GL_POSITION, position);

    glLightf(GL_LIGHT0 + light->id, GL_CONSTANT_ATTENUATION , light->constant );
    glLightf(GL_LIGHT0 + light->id, GL_LINEAR_ATTENUATION   , light->linear   );
    glLightf(GL_LIGHT0 + light->id, GL_QUADRATIC_ATTENUATION, light->quadratic);
}