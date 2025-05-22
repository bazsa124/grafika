#include "scene.h"
#include "fog.h"
#include <stdlib.h>

// ================================================================================
// Thing update functions.

void camera_update(Thing *thing, Scene *scene)
{
    // Keep camera in bounds.
    scene->camera->position.x = clamp(scene->camera->position.x, -32.0f, 32.0f);
    scene->camera->position.y = clamp(scene->camera->position.y, -32.0f, 32.0f);

    // Predict the new camera position (already set by input earlier)
    vec3 desired_position = scene->camera->position;

    // Create predicted bounds for the camera at new position
    Sphere new_bounds = thing->bounds;
    new_bounds.center = desired_position;

    bool collision = false;

    Node *node = scene->things->head;
    while (node != NULL)
    {
        Thing *other = node->data;
        node = node->next;

        // Skip self and things without bounds/extra_data
        if (other == thing || other->extra_data == NULL)
            continue;

        float dist = len_vec3(sub_vec3(other->position, desired_position));
        float min_dist = new_bounds.radius + other->bounds.radius;

        if (dist < min_dist)
        {
            collision = true;
            break;
        }
    }

    if (!collision)
    {
        thing->position = desired_position;
    }
    else
    {
        // Revert the camera's position to the Thing (dummy) if blocked
        scene->camera->position = thing->position;
    }

    // Sync rotation and bounds
    thing->rotation.z = scene->camera->rotation.z;
    thing->bounds.center = thing->position;
}

void zombie_update(Thing *thing, Scene *scene)
{
    // Suppress warnings.
    (void)scene;

    EnemyData *enemy = (EnemyData *)thing->extra_data;

    // Look at camera
    vec3 base_dir = sub_vec3(scene->camera->position, thing->position);
    vec3 direction = norm_vec3(base_dir);
    float angle = radian_to_degree(atan2f(direction.y, direction.x));
    float target_angle = angle + 90.0f;
    float current_angle = thing->rotation.y;
    float diff = fmodf(target_angle - current_angle + 540.0f, 360.0f) - 180.0f;
    float max_diff = 180.0f * scene->delta_time;
    diff = clamp(diff, -max_diff, max_diff);
    thing->rotation.y = current_angle + diff;

    // Movement
    if (len_vec3(base_dir) > 1.0f)
    {
        vec3 move = mul_vec3(direction, 1.0f * scene->delta_time);
        bool collision = false;

        // Update intended position and bounds for collision check
        vec3 new_pos = add_vec3(thing->position, move);
        Sphere new_bounds = thing->bounds;
        new_bounds.center = new_pos;

        // Check for collisions with other things
        Node *node = scene->things->head;
        while (node != NULL)
        {
            Thing *other = node->data;
            node = node->next;

            if (other == thing || other->extra_data == NULL || other->is_visible==false)
            {
                continue;
            }

            float dist = len_vec3(sub_vec3(other->position, new_pos));
            float min_dist = new_bounds.radius + other->bounds.radius;

            if (dist < min_dist)
            {
                collision = true;
                break;
            }
        }

        // Apply move if no collision and not too close to player
        if (!collision)
        {
            thing->position = new_pos;
            thing->position.z = -2.7f;
            thing->bounds.center = thing->position;
        }
    }
    else
    {
        enemy->timer -= scene->delta_time;
    }
}
// ================================================================================
// Animations
void sun_animation(Thing *thing, Scene *scene)
{
    (void)scene;

    float time = scene->time;
    float radius = 50.0f; // Orbit radius
    float speed = 0.05f;  // Orbit speed multiplier (radians per second)

    float angle = speed * time;

    // Simple circular orbit in the XZ plane, rising and falling in Y slightly
    thing->position.x = radius * cosf(angle);
    thing->position.z = radius * sinf(angle);
    thing->position.y = 5.0f + 3.0f * sinf(angle); // slight up/down oscillation
}

// ================================================================================
// Generate enemies
void generate_enemies(Scene *scene, int number)
{
    Thing *zombies[number];

    vec3 cam_pos = scene->camera->position;

    for (int i = 0; i < number; i++)
    {
        // Generáljunk távoli, random pozíciót legalább 3 egységre a kamerától
        vec3 pos;
        float dist;

        do
        {
            // Válasszunk véletlen X/Y értéket -10 és 10 között, Z legyen fix
            pos.x = ((float)rand() / RAND_MAX) * 20.0f - 10.0f;
            pos.y = ((float)rand() / RAND_MAX) * 20.0f - 10.0f;
            pos.z = -2.7f;

            float dx = pos.x - cam_pos.x;
            float dy = pos.y - cam_pos.y;
            float dz = pos.z - cam_pos.z;

            dist = sqrtf(dx * dx + dy * dy + dz * dz);
        } while (dist < 5.0f);

        // Hozzáadás
        zombies[i] = add_node(scene->things, new_thing());
        load_thing(zombies[i], "assets/models/zombie.obj", "assets/textures/zombie.tga");

        zombies[i]->position = pos;
        zombies[i]->scale = new_vec3(1.0f, 1.0f, 1.0f);
        zombies[i]->rotation.x = 90.0f;
        set_update_function(zombies[i], zombie_update);
        zombies[i]->extra_data = (EnemyData *)malloc(sizeof(EnemyData));
        ((EnemyData *)zombies[i]->extra_data)->health = 100.0f;
        ((EnemyData *)zombies[i]->extra_data)->state = ENEMY_MOVE;

        zombies[i]->bounds.center = zombies[i]->position;
        zombies[i]->bounds.radius = 0.5f * zombies[i]->scale.x;
    }
}

void exit_game(UI_Element *element, Scene *scene)
{
    (void)element;
    (void)scene;

    exit(0);
}
void pre_restart(Scene *scene, Camera *camera, int window_width, int window_height)
{
    camera->base_speed = 2.0f;
    camera->position = new_vec3(0.0f, 0.0f, 0.0f);

    scene->camera = camera;
    scene->window_width = window_width;
    scene->window_height = window_height;

    scene->delta_time = 0.0f;
    scene->time = 0.0f;

    scene->player_health = 100.0f;
    scene->attack_timer = 0.0f;

    scene->enemy_count = 3;
}

void post_restart(Scene *scene)
{
    printf("Restarting the game.\n");

    // Reset the player health.
    scene->player_health = 100.0f;
    scene->health_bar->size.x = 256.0f;
}

// ================================================================================
// Scene initialization.

void init_scene(Scene *scene, Camera *camera, int window_width, int window_height)
{
    // ================================================================================

    scene->things = new_linked_list();
    init_linked_list(scene->things);
    scene->lights = new_linked_list();
    init_linked_list(scene->lights);
    scene->ui_elements = new_linked_list();
    init_linked_list(scene->ui_elements);

    pre_restart(scene, camera, window_width, window_height);

    // ================================================================================

    Thing *camera_dummy = add_node(scene->things, new_thing());
    camera_dummy->bounds.center = camera_dummy->position;
    camera_dummy->bounds.radius = 1.0f;
    init_thing(camera_dummy, NULL, NULL, -1);
    set_update_function(camera_dummy, camera_update);

    Thing *floor = add_node(scene->things, new_thing());
    load_thing(floor, "assets/models/grass.obj", "assets/textures/grass_field.jpg");
    floor->position = new_vec3(0.0f, 0.0f, -3.0f);
    floor->scale = new_vec3(2.0f, 2.0f, 0.0f);

    Thing *walls = add_node(scene->things, new_thing());
    load_thing(walls, "assets/models/forest.obj", "assets/textures/forest.jpg");
    walls->position = new_vec3(0.0f, 0.0f, -8.0f);
    walls->scale = new_vec3(2.0f, 2.0f, 3.0f);

    Thing *sun = add_node(scene->things, new_thing());
    load_thing(sun, "assets/models/sol.obj", "assets/textures/sol.jpg");
    sun->position = new_vec3(0.0f, 0.0f, 80.0f);
    sun->scale = new_vec3(2.0f, 2.0f, 2.0f);
    set_update_function(sun, sun_animation);

    Thing *gun = add_node(scene->things, new_thing());
    load_thing(gun, "assets/models/Gun.obj", "assets/textures/Gun.png");
    gun->position = new_vec3(0.4f, -0.4f, -0.2f);
    gun->scale = new_vec3(1.0f, 1.0f, 1.0f);
    gun->rotation.x = 70;
    gun->rotation.y = -160;
    gun->rotation.z = -20;
    set_thing_parent(gun, camera_dummy);
    // Enemy

    generate_enemies(scene, 20);

    // Exit button.
    GLuint exit_button_texture = load_texture("assets/textures/ui/Exit.png");
    UI_Element *ui_exit_button = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(ui_exit_button, new_vec3(32.0f, 32.0f, 0.0f), new_vec3(64.0f, 64.0f, 1.0f), exit_button_texture);
    ui_exit_button->anchor_x = UI_ANCHOR_FAR;
    ui_exit_button->anchor_y = UI_ANCHOR_CLOSE;
    set_ui_element_mouse_click(ui_exit_button, exit_game);

    //================================================================================
    // Lights

    Light *player_light = add_node(scene->lights, new_light());
    init_light(player_light, 0);
    set_light_position(player_light, vec3_zero());
    set_light_parent(player_light, camera_dummy);
    set_light_color(player_light, new_vec3(0.1f, 0.1f, 0.1f), new_vec3(1.0f, 0.9f, 0.8f), new_vec3(1.0f, 0.9f, 0.8f));
    player_light->strength = 1.0f;

    Light *sun_light = add_node(scene->lights, new_light());
    init_light(sun_light, 1);
    set_light_position(sun_light, vec3_zero());
    set_light_parent(sun_light, sun);
    set_light_color(sun_light, new_vec3(0.1f, 0.1f, 0.1f), new_vec3(1.0f, 0.9f, 0.8f), new_vec3(1.0f, 0.9f, 0.8f));
    sun_light->strength = 75.0f;

    //================================================================================
    // Fog settings

    // init_fog();
    // set_fog_color(new_vec3(0.1f, 0.1f, 0.1f));
    // scene->fog_density = 0.25f;
    // set_fog_density(scene->fog_density);
    // glEnable(GL_CULL_FACE);
}

// ================================================================================
// Scene event handling.

void handle_jump(Scene *scene, bool is_jumping)
{
    if (scene->camera->position.z < 0.01f && is_jumping)
    {
        scene->jump_speed = 5.0f;
        scene->camera->position.z = 0.015f;
    }
    else
    {
        if (scene->camera->position.z > 0.0f)
        {
            scene->jump_speed -= 9.8f * scene->delta_time;
            scene->camera->position.z += scene->jump_speed * scene->delta_time;
        }
    }
}

void handle_mouse_click(Scene *scene, int x, int y)
{
    for (Node *node = scene->ui_elements->head; node != NULL; node = node->next)
    {
        UI_Element *element = (UI_Element *)node->data;

        if (!element->is_visible)
            continue;

        if (is_mouse_over_ui_element(element, new_vec3(x, y, 0.0f), scene->window_width, scene->window_height))
        {
            on_click(element, scene);
        }
    }
}

void handle_attack(Scene *scene)
{
    vec3 camera_pos = scene->camera->position;
    float angle_rad = scene->camera->rotation.z * (3.14159265f / 180.0f);
    vec3 facing_dir = new_vec3(cosf(angle_rad), sinf(angle_rad), 0.0f);

    const float ATTACK_RANGE = 10.0f;
    const float DOT_THRESHOLD = 0.8f;

    Node *node = scene->things->head;
    Thing *closest_enemy = NULL;
    float closest_dist = ATTACK_RANGE + 1.0f;

    while (node != NULL)
    {
        Thing *other = node->data;
        node = node->next;

        if (other->extra_data == NULL || other->is_visible==false)
            continue;

        vec3 to_enemy = sub_vec3(other->position, camera_pos);
        to_enemy.z = 0.0f;
        float distance = len_vec3(to_enemy);
        if (distance > ATTACK_RANGE)
            continue;

        vec3 dir_to_enemy = norm_vec3(to_enemy);
        float dot = dir_to_enemy.x * facing_dir.x + dir_to_enemy.y * facing_dir.y;
        if (dot > DOT_THRESHOLD && distance < closest_dist)
        {
            closest_dist = distance;
            closest_enemy = other;
        }
    }

    if (closest_enemy != NULL)
    {
        EnemyData *enemy = (EnemyData *)closest_enemy->extra_data;
        enemy->health -= 100.0f;

        printf("[DEBUG] HIT closest enemy at (%.2f, %.2f)! Health now: %d\n",
               closest_enemy->position.x, closest_enemy->position.y, enemy->health);

        if (enemy->health <= 0.0f)
        {
            closest_enemy->is_visible = false;
            enemy->state = ENEMY_DEAD;
            printf("[DEBUG] Enemy died.\n");
        }
        else
        {
            enemy->state = ENEMY_PAIN;
            enemy->timer = 0.5f;
            printf("[DEBUG] Enemy hurt.\n");
        }
    }
    else
    {
        printf("[DEBUG] No enemy hit.\n");
    }
}


void handle_scene_events(Scene *scene, SDL_Event *event)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
    {
        if (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL])
        {
            handle_mouse_click(scene, event->button.x, event->button.y);
        }
        else
        {
            handle_attack(scene);
        }
    }
    if (event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.scancode)
        {
        case SDL_SCANCODE_SPACE:
            handle_jump(scene, true);
            break;
        case SDL_SCANCODE_R:
            pre_restart(scene, scene->camera, scene->window_width, scene->window_height);
            post_restart(scene);
            break;
        default:
            break;
        }
    }
}

// ================================================================================
// Scene update and rendering.

void update_scene(Scene *scene)
{
    for (Node *node = scene->things->head; node != NULL; node = node->next)
    {
        Thing *thing = (Thing *)node->data;
        update_thing(thing, scene);
    }
    handle_jump(scene, false);
}

void render_scene(Scene *scene, float delta_time)
{
    scene->delta_time = delta_time;
    scene->time += delta_time;

    for (Node *node = scene->lights->head; node != NULL; node = node->next)
    {
        Light *light = (Light *)node->data;
        set_light(light);
    }

    for (Node *node = scene->things->head; node != NULL; node = node->next)
    {
        Thing *thing = (Thing *)node->data;
        render_thing(thing, scene);
    }

    set_2d_render_mode(scene->window_width, scene->window_height);

    for (Node *node = scene->ui_elements->head; node != NULL; node = node->next)
    {
        UI_Element *element = (UI_Element *)node->data;
        render_ui_element(element, scene->window_width, scene->window_height);
    }

    reset_render_mode();
}
