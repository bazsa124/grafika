#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "thing.h"
#include "light.h"
#include "linkedlist.h"
#include "ui.h"

#include <SDL2/SDL.h>

typedef enum EnemyState
{
    ENEMY_MOVE,
    ENEMY_ATTACK,
    ENEMY_PAIN,
    ENEMY_DEAD
} EnemyState;

typedef struct EnemyData
{
    EnemyState state;
    int health;
    float timer;
} EnemyData;

struct Scene
{
    float delta_time;
    float time;

    LinkedList *things;
    LinkedList *lights;
    LinkedList *ui_elements;
    Camera *camera;

    int window_width;
    int window_height;

    float fog_density;

    UI_Element *health_bar;
    UI_Element *game_over_text;
    UI_Element *victory_text;
    float player_health;

    float attack_timer;
    float jump_speed;
    int enemy_count;
};

/**
 * Initialize the scene by loading models.
 */
void init_scene(Scene *scene, Camera *camera, int window_width, int window_height);

/**
 * Handle the scene events.
 */
void handle_scene_events(Scene *scene, SDL_Event *event);

/**
 * Update the scene.
 */
void update_scene(Scene *scene);

/**
 * Render the scene objects.
 */
void render_scene(Scene *scene, float delta_time);

#endif /* SCENE_H */
