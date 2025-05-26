#ifndef UI_CONTROLS_H
#define UI_CONTROLS_H

#include "scene.h"
#include "fog.h"

void increase_camera_speed(UI_Element *element, Scene *scene);
void decrease_camera_speed(UI_Element *element, Scene *scene);
void increase_light_strength(UI_Element *element, Scene *scene);
void decrease_light_strength(UI_Element *element, Scene *scene);
void increase_fog_density(UI_Element *element, Scene *scene);
void decrease_fog_density(UI_Element *element, Scene *scene);
void toggle_help(UI_Element *element, Scene *scene);
void exit_game(UI_Element *element, Scene *scene);

#endif