#ifndef THING_H
#define THING_H

#include "load.h"
#include "draw.h"
#include "model.h"
#include "texture.h"
#include "utils.h"

#include <stdbool.h>
#include <stdlib.h>

typedef struct Thing Thing;
typedef struct Scene Scene;
typedef struct Sphere Sphere;

typedef void (*RenderFunction)(Thing* thing, Scene* scene);
typedef void (*UpdateFunction)(Thing* thing, Scene* scene);

struct Sphere {
    vec3 center;
    float radius;
};


struct Thing
{
    Model model;
    Material material;
    GLuint texture_id;

    vec3 position;
    vec3 rotation;
    vec3 scale;

    bool is_transparent;
    bool is_enabled;
    bool is_visible;

    Sphere bounds;

    Thing* parent;

    RenderFunction render;
    UpdateFunction update;

    void* extra_data;
};

/**
 * Allocate a new thing.
 */
Thing* new_thing();

/**
 * Initialize the thing.
 */
void init_thing(Thing* thing, Model* model, Material* material, GLuint texture_id);

/**
 * Initialize the thing with a model and texture path. 
 */
void load_thing(Thing* thing, const char* model_path, const char* texture_path);

/**
 * Set the thing's position, rotation and scale.
 */
void set_thing_transform(Thing* thing, vec3 position, vec3 rotation, vec3 scale);

/**
 * Initialize the material.
 */
void init_material(Material* material);

/**
 * Set the thing's parent.
 */
void set_thing_parent(Thing* thing, Thing* parent);

/**
 * Set the thing's render function.
 */
void set_render_function(Thing* thing, RenderFunction render);

/**
 * Set the thing's update function.
 */
void set_update_function(Thing* thing, UpdateFunction update);

/**
 * Update the thing.
 */
void update_thing(Thing* thing, Scene* scene);

/**
 * Render the thing.
 */
void render_thing(Thing* thing, Scene* scene);

/**
 * Apply the thing's transformation with respect to its parent.
 */
void apply_transform(Thing* thing);

/**
 * Pop the thing's transformation with respect to its parent.
 */
void pop_transform(Thing* thing);

/**
 * A default render function for things.
 */
void internal_render_thing(Thing* thing, Scene* scene);

bool sphere_collision(Sphere a, Sphere b);

#endif /* THING_H */