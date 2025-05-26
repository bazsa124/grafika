#include "enviroment.h"
void toggle_help(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;

    UI_Element *help = get_node(scene->ui_elements, 0);
    help->is_visible = !help->is_visible;
}

void exit_game(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;
    (void)scene;

    exit(0);
}

void increase_camera_speed(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;

    scene->camera->base_speed += 0.5f;
}

void decrease_camera_speed(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;

    if (scene->camera->base_speed > 1.01f)
        scene->camera->base_speed -= 0.5f;
}

void increase_light_strength(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;

    Light *light = get_node(scene->lights, 0);
    light->strength += 1.0f;
}

void decrease_light_strength(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;

    Light *light = get_node(scene->lights, 0);
    if (light->strength > 1.01f)
        light->strength -= 1.0f;
}

void increase_fog_density(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;

    scene->fog_density += 0.1f;
    set_fog_density(scene->fog_density);
}

void decrease_fog_density(UI_Element *element, Scene *scene)
{
    // Suppress warnings.
    (void)element;

    if (scene->fog_density > 0.01f)
    {
        scene->fog_density -= 0.1f;
        set_fog_density(scene->fog_density);
    }
}