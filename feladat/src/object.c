#include "object.h"

#include <stdio.h>
#include <stdlib.h>

Thing* new_thing()
{
    return (Thing*)malloc(sizeof(Thing));
}

void init_thing(Thing* thing, Model* model, Material* material, GLuint texture_id)
{
    if (model && material && texture_id != (GLuint)(-1)) {
        thing->model      = *model;
        thing->material   = *material;
        thing->texture_id = texture_id;
        thing->is_visible = true;
    } else {
        thing->is_visible = false;
    }

    thing->position = vec3_zero();
    thing->rotation = vec3_zero();
    thing->scale    = vec3_one();

    thing->parent   = NULL;

    thing->is_transparent = false;
    thing->is_enabled     = true;

    thing->render   = internal_render_thing;
    thing->update   = NULL;

    thing->extra_data = NULL;
}

void load_thing(Thing* thing, const char* model_path, const char* texture_path)
{
    Model*    model      = (Model*)   malloc(sizeof(Model)   );
    Material* material   = (Material*)malloc(sizeof(Material));
    GLuint    texture_id = -1;

    texture_id = load_texture (texture_path);
                 load_model   (model, model_path);
                 init_material(material);

    init_thing(thing, model, material, texture_id);
}

void set_thing_transform(Thing* thing, vec3 position, vec3 rotation, vec3 scale)
{
    thing->position = position;
    thing->rotation = rotation;
    thing->scale    = scale;
}

void init_material(Material* material)
{
    material->ambient.x = 0.1f;
    material->ambient.y = 0.1f;
    material->ambient.z = 0.1f;

    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;

    material->specular.x = 1.0f;
    material->specular.y = 1.0f;
    material->specular.z = 1.0f;

    material->emission.x = 0.0f;
    material->emission.y = 0.0f;
    material->emission.z = 0.0f;

    material->shininess  = 64.0f;
}

void set_material(const Material* material)
{
    float ambient_material_color[] = {
        material->ambient.x,
        material->ambient.y,
        material->ambient.z
    };

    float diffuse_material_color[] = {
        material->diffuse.x,
        material->diffuse.y,
        material->diffuse.z
    };

    float specular_material_color[] = {
        material->specular.x,
        material->specular.y,
        material->specular.z
    };

    float emission_material_color[] = {
        material->emission.x,
        material->emission.y,
        material->emission.z
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT , ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE , diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission_material_color);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void set_thing_parent(Thing* thing, Thing* parent)
{
    thing->parent = parent;
}

void set_render_function(Thing* thing, RenderFunction render)
{
    thing->render = render;
}

void set_update_function(Thing* thing, UpdateFunction update)
{
    thing->update = update;
}

void update_thing(Thing* thing, Scene* scene)
{
    if (thing->update && thing->is_enabled)
    {
        thing->update(thing, scene);
    }
}

void render_thing(Thing* thing, Scene* scene)
{
    if (thing->render && thing->is_visible)
    {
        thing->render(thing, scene);
    }
}

void apply_transform(Thing* thing)
{
    glPushMatrix();

    if (thing->parent)
    {
        apply_transform(thing->parent);
    }

    glTranslatef(thing->position.x, thing->position.y, thing->position.z);
    glRotatef(thing->rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(thing->rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(thing->rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(thing->scale.x, thing->scale.y, thing->scale.z);
}

void pop_transform(Thing* thing)
{
    glPopMatrix();

    if (thing->parent)
    {
        pop_transform(thing->parent);
    }
}

void internal_render_thing(Thing* thing, Scene* scene)
{
    // Suppress warnings.
    (void)scene;

    apply_transform(thing);

    if (thing->is_transparent)
    {
        glEnable(GL_BLEND);
        //glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
    }

    set_material(&thing->material);
    glBindTexture(GL_TEXTURE_2D, thing->texture_id);

    draw_model(&thing->model);

    if (thing->is_transparent)
    {
        glDisable(GL_BLEND);
        //glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    pop_transform(thing);
}

bool sphere_collision(Sphere a, Sphere b) {
    float dist = len_vec3(sub_vec3(a.center, b.center));
    return dist < (a.radius + b.radius);
}