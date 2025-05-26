#include "scene.h"
#include "fog.h"
#include <stdlib.h>
#include "enviroment.h"
// Enviroment

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
        if (other == thing || other->extra_data == NULL || other == scene->gun)
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
    EnemyData *enemy = (EnemyData *)thing->extra_data;

    // Calculate direction and angle to camera
    vec3 base_dir = sub_vec3(scene->camera->position, thing->position);
    vec3 direction = norm_vec3(base_dir);
    float angle = radian_to_degree(atan2f(direction.y, direction.x));
    float target_angle = angle + 90.0f;
    float current_angle = thing->rotation.y;

    // Smooth angle adjustment
    float diff = fmodf(target_angle - current_angle + 540.0f, 360.0f) - 180.0f;
    float max_diff = 180.0f * scene->delta_time;
    diff = clamp(diff, -max_diff, max_diff);
    thing->rotation.y = current_angle + diff;

    // State machine logic
    switch (enemy->state)
    {
    case ENEMY_MOVE:
    {
        if (len_vec3(base_dir) > 3.0f)
        {
            vec3 move = mul_vec3(direction, 2.0f * scene->delta_time);
            bool collision = false;
            vec3 new_pos = add_vec3(thing->position, move);
            Sphere new_bounds = thing->bounds;
            new_bounds.center = new_pos;

            Node *node = scene->things->head;
            while (node != NULL)
            {
                Thing *other = node->data;
                node = node->next;

                if (other == thing || other->extra_data == NULL || !other->is_visible)
                    continue;

                float dist = len_vec3(sub_vec3(other->position, new_pos));
                float min_dist = new_bounds.radius + other->bounds.radius;

                if (dist < min_dist)
                {
                    collision = true;
                    break;
                }
            }

            if (!collision)
            {
                thing->position = new_pos;
                thing->position.z = -2.7f;
                thing->bounds.center = thing->position;
            }
        }
        else
        {
            scene->player_health -= 10.0f;
            enemy->state = ENEMY_ATTACK;
            enemy->timer = 0.5f;

            if (scene->player_health < 0.0f)
            {
                scene->player_health = 0.0f;
                scene->game_over_text->is_visible = true;
                scene->camera->base_speed = 0.0f;
                scene->attack_timer = -1.0f;
            }

            scene->health_bar->size.x = scene->player_health * 2.56f;
        }
        break;
    }

    case ENEMY_ATTACK:
    {

        thing->rotation.y = target_angle + 15.0f * sinf(scene->time * 2.0f);

        enemy->timer -= scene->delta_time;
        if (enemy->timer <= 0.0f)
        {
            enemy->state = ENEMY_MOVE;

            enemy->timer = 0.0f;
            thing->rotation.y = 0.0f;
        }
        break;
    }

    case ENEMY_PAIN:
    {
        // Compute backward direction based on current rotation
        float radians = degree_to_radian(thing->rotation.y - 90.0f);
        vec3 backward = new_vec3(cosf(radians), sinf(radians), 0.0f);
        vec3 move = mul_vec3(backward, -2.0f * scene->delta_time);

        // Predict new position
        vec3 new_pos = add_vec3(thing->position, move);
        Sphere new_bounds = thing->bounds;
        new_bounds.center = new_pos;

        bool collision = false;

        // Check for collisions with other things
        Node *node = scene->things->head;
        while (node != NULL)
        {
            Thing *other = node->data;
            node = node->next;

            if (other == thing || !other->is_visible || other->extra_data == NULL)
                continue;

            float dist = len_vec3(sub_vec3(other->position, new_pos));
            float min_dist = new_bounds.radius + other->bounds.radius;

            if (dist < min_dist)
            {
                collision = true;
                break;
            }
        }

        // Only apply backward movement if no collision
        if (!collision)
        {
            thing->position = new_pos;
            thing->bounds.center = thing->position;
        }

        // Always turn red
        thing->material.ambient = new_vec3(1.0f, 0.0f, 0.0f);

        // Countdown timer
        enemy->timer -= scene->delta_time;
        if (enemy->timer <= 0.0f)
        {
            enemy->state = ENEMY_MOVE;
            enemy->timer = 0.0f;
            thing->material.ambient = new_vec3(1.0f, 1.0f, 1.0f); // Reset color
        }
        break;
    }

    case ENEMY_DEAD:
    {
        float target_angle = 90.0f;
        float current_angle = thing->rotation.z;

        float diff = fmodf(target_angle - current_angle + 540.0f, 360.0f) - 180.0f;

        float rotation_speed = 90.0f;

        float max_step = rotation_speed * scene->delta_time;
        if (fabsf(diff) < max_step)
        {
            thing->rotation.z = target_angle;
            thing->is_enabled = false;
            thing->is_visible = false;
        }
        else
        {
            float step = (diff > 0.0f) ? max_step : -max_step;
            thing->rotation.z += step;
        }

        enemy->timer -= scene->delta_time;
        break;
    }

    default:
        break;
    }
}

void gun_update(Thing *gun, Scene *scene)
{
    for (int i = 0; i < 7; ++i)
    {
        GunData *gun_data = (GunData *)scene->gun->extra_data;
        for (int i = 0; i < 7; ++i)
            gun_data->counters[i]->is_visible = (i == scene->bullets);

        GunState *state = &((GunData *)gun->extra_data)->state;
        static float anim_timer = 0.0f;

        const float shoot_duration = 1.0f;
        const float reload_duration = 3.0f;

        // Def rots and pos
        const vec3 default_pos = {0.4f, -0.4f, -0.2f};
        const float base_rot_x = 70.0f;
        const float base_rot_y = -160.0f;
        const float base_rot_z = -20.0f;

        if (*state == SHOOT)
        {
            anim_timer += scene->delta_time;
            float t = anim_timer / shoot_duration;
            if (t > 1.0f)
                t = 1.0f;

            // Animate offset: tilt up by 15 degrees then back
            float angle_offset = -15.0f * sinf(t * 3.14159f);

            // Apply rotation relative to base
            gun->rotation.x = base_rot_x;
            gun->rotation.y = base_rot_y;
            gun->rotation.z = base_rot_z + angle_offset;

            if (anim_timer >= shoot_duration)
            {
                *state = STANDBY;
                anim_timer = 0.0f;

                // Reset rotation to base
                gun->rotation.x = base_rot_x;
                gun->rotation.y = base_rot_y;
                gun->rotation.z = base_rot_z;
            }
        }
        else if (*state == STANDBY)
        {
            gun->position = default_pos;
            gun->rotation.x = base_rot_x;
            gun->rotation.y = base_rot_y;
            gun->rotation.z = base_rot_z;
            anim_timer = 0.0f;
        }
        else if (*state == RELOAD)
        {
            anim_timer += scene->delta_time;
            float t = anim_timer / reload_duration;
            if (t > 1.0f)
                t = 1.0f;

            // Y movement (down and back up)
            float move_z = -0.08f * sinf(t * 3.14159f); // from 0 to -0.08 to 0
            // X rotation (tilt and return)
            float rot_z = base_rot_z + 40.0f * sinf(t * 3.14159f);

            gun->position = new_vec3(default_pos.x, default_pos.y, default_pos.z + move_z);
            gun->rotation.x = base_rot_x;
            gun->rotation.y = base_rot_y;
            gun->rotation.z = rot_z;

            if (anim_timer >= reload_duration)
            {
                *state = STANDBY;
                scene->bullets = 6;
                anim_timer = 0.0f;
                gun->position = default_pos;
                gun->rotation.x = base_rot_x;
                gun->rotation.y = base_rot_y;
                gun->rotation.z = base_rot_z;
            }
        }
    }
}

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

    scene->enemy_count = 20;
    scene->bullets = 6;
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
    scene->gun = gun;
    load_thing(gun, "assets/models/Gun.obj", "assets/textures/Gun.png");
    gun->position = new_vec3(0.4f, -0.4f, -0.2f);
    gun->scale = new_vec3(1.0f, 1.0f, 1.0f);
    set_thing_parent(gun, camera_dummy);
    gun->extra_data = (GunData *)malloc(sizeof(GunData));
    ((GunData *)gun->extra_data)->state = STANDBY;
    set_update_function(gun, gun_update);
    // Enemy

    generate_enemies(scene, 20);
    //==========================================================
    // Help
    GLuint help_text = load_texture("assets/textures/ui/Help.png");
    UI_Element *ui_help = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(ui_help, new_vec3(0.0f, 0.0f, 0.0f), new_vec3(1280.0f, 720.0f, 1.0f), help_text);
    ui_help->is_visible = false;

    // Exit button.
    GLuint exit_button_texture = load_texture("assets/textures/ui/Exit.png");
    UI_Element *ui_exit_button = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(ui_exit_button, new_vec3(32.0f, 32.0f, 0.0f), new_vec3(64.0f, 64.0f, 1.0f), exit_button_texture);
    ui_exit_button->anchor_x = UI_ANCHOR_FAR;
    ui_exit_button->anchor_y = UI_ANCHOR_CLOSE;
    set_ui_element_mouse_click(ui_exit_button, exit_game);

    GLuint help_button_texture = load_texture("assets/textures/ui/help_icon.png");
    UI_Element *ui_help_button = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(ui_help_button, new_vec3(32.0f, 32.0f, 0.0f), new_vec3(64.0f, 64.0f, 1.0f), help_button_texture);
    ui_help_button->anchor_x = UI_ANCHOR_CLOSE;
    ui_help_button->anchor_y = UI_ANCHOR_CLOSE;
    set_ui_element_mouse_click(ui_help_button, toggle_help);

    GLuint health_bar_texture = load_texture("assets/textures/ui/Health.png");
    scene->health_bar = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(scene->health_bar, new_vec3(0.0f, 32.0f, 0.0f), new_vec3(256.0f, 32.0f, 1.0f), health_bar_texture);
    scene->health_bar->anchor_x = UI_ANCHOR_CENTER;
    scene->health_bar->anchor_y = UI_ANCHOR_FAR;

    GLuint bullet_pic_texture = load_texture("assets/textures/ui/bullet/bullet_pic.png");
    UI_Element *ui_bullet_pic = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(ui_bullet_pic, new_vec3(5.0f, 32.0f, 0.0f), new_vec3(32.0f, 32.0f, 1.0f), bullet_pic_texture);
    ui_bullet_pic->anchor_x = UI_ANCHOR_FAR;
    ui_bullet_pic->anchor_y = UI_ANCHOR_FAR;

    GLuint bullet_right_texture = load_texture("assets/textures/ui/bullet/bullet_right.png");
    UI_Element *ui_bullet_right = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(ui_bullet_right, new_vec3(20.0f, 32.0f, 0.0f), new_vec3(32.0f, 32.0f, 1.0f), bullet_right_texture);
    ui_bullet_right->anchor_x = UI_ANCHOR_FAR;
    ui_bullet_right->anchor_y = UI_ANCHOR_FAR;

    const char *bullet_texture_paths[7] = {
        "assets/textures/ui/bullet/bullet_left_0.png",
        "assets/textures/ui/bullet/bullet_left_1.png",
        "assets/textures/ui/bullet/bullet_left_2.png",
        "assets/textures/ui/bullet/bullet_left_3.png",
        "assets/textures/ui/bullet/bullet_left_4.png",
        "assets/textures/ui/bullet/bullet_left_5.png",
        "assets/textures/ui/bullet/bullet_left_6.png"};

    for (int i = 0; i < 7; ++i)
    {
        UI_Element *ui_bullet_left = add_node(scene->ui_elements, new_ui_element());

        init_ui_element(
            ui_bullet_left,
            new_vec3(50.0f, 32.0f, 0.0f),         // Position
            new_vec3(32.0f, 32.0f, 1.0f),         // Scale
            load_texture(bullet_texture_paths[i]) // Texture
        );

        ui_bullet_left->anchor_x = UI_ANCHOR_FAR;
        ui_bullet_left->anchor_y = UI_ANCHOR_FAR;
        ui_bullet_left->is_visible = false;
        ((GunData *)scene->gun->extra_data)->counters[i] = ui_bullet_left;
    }

    GLuint end_text = load_texture("assets/textures/ui/lose.png");
    scene->game_over_text = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(scene->game_over_text, new_vec3(0.0f, 0.0f, 0.0f), new_vec3(1280.0f, 720.0f, 1.0f), end_text);
    scene->game_over_text->is_visible = false;

    GLuint victory_texture = load_texture("assets/textures/ui/win.png");
    scene->victory_text = add_node(scene->ui_elements, new_ui_element());
    init_ui_element(scene->victory_text, new_vec3(0.0f, 0.0f, 0.0f), new_vec3(1280.0f, 720.0f, 1.0f), victory_texture);
    scene->victory_text->is_visible = false;

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

    init_fog();
    set_fog_color(new_vec3(0.1f, 0.1f, 0.1f));
    scene->fog_density = 0.25f;
    set_fog_density(scene->fog_density);
    glEnable(GL_CULL_FACE);
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
    printf("Shoot!\n");
    if (scene->player_health <= 0.0f || scene->gun == NULL || scene->gun->extra_data == NULL)
        return;

    GunState *gun_state = &((GunData *)scene->gun->extra_data)->state;

    // Only allow shooting if gun is in STANDBY (ready) state
    if (*gun_state != STANDBY)
        return;

    vec3 camera_pos = scene->camera->position;
    float angle_rad = scene->camera->rotation.z * (3.14159265f / 180.0f);
    vec3 facing_dir = new_vec3(cosf(angle_rad), sinf(angle_rad), 0.0f);

    const float ATTACK_RANGE = 10.0f;
    // Side angle, the higher the less the acceptable
    const float DOT_THRESHOLD = 0.95f;

    Node *node = scene->things->head;
    Thing *closest_enemy = NULL;
    float closest_dist = ATTACK_RANGE + 1.0f;

    while (node != NULL)
    {
        Thing *other = node->data;
        node = node->next;

        if (other->extra_data == NULL || other->is_visible == false)
            continue;

        EnemyData *enemy = (EnemyData *)other->extra_data;
        if (enemy->state == ENEMY_DEAD || enemy->health <= 0.0f)
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
        enemy->health -= 50.0f;
        scene->bullets--;
        printf("[DEBUG] HIT closest enemy at (%.2f, %.2f)! Health now: %d, Remaining bullet: %d\n",
               closest_enemy->position.x, closest_enemy->position.y, enemy->health, scene->bullets);

        if (enemy->health <= 0.0f)
        {
            enemy->state = ENEMY_DEAD;
            scene->enemy_count--;
            enemy->timer = 0.1f;
            printf("[DEBUG] Enemy died. Remaining: %d\n", scene->enemy_count);
            if (scene->enemy_count == 0)
            {
                scene->victory_text->is_visible = true;
            }
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

    // Set gun state to SHOOT to trigger shooting animation
    if (scene->bullets <= 0)
    {
        *gun_state = RELOAD;
        scene->bullets = 0; // clamp to 0, just in case
    }
    else
    {
        *gun_state = SHOOT;
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
        const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
        SDL_Scancode key = event->key.keysym.scancode;

        if ((key == SDL_SCANCODE_EQUALS || key == SDL_SCANCODE_KP_PLUS) && keyboard_state[SDL_SCANCODE_C])
        {
            increase_camera_speed(NULL, scene);
        }
        else if ((key == SDL_SCANCODE_MINUS || key == SDL_SCANCODE_KP_MINUS) && keyboard_state[SDL_SCANCODE_C])
        {
            decrease_camera_speed(NULL, scene);
        }
        else if ((key == SDL_SCANCODE_EQUALS || key == SDL_SCANCODE_KP_PLUS) && keyboard_state[SDL_SCANCODE_L])
        {
            increase_light_strength(NULL, scene);
        }
        else if ((key == SDL_SCANCODE_MINUS || key == SDL_SCANCODE_KP_MINUS) && keyboard_state[SDL_SCANCODE_L])
        {
            decrease_light_strength(NULL, scene);
        }
        else if ((key == SDL_SCANCODE_EQUALS || key == SDL_SCANCODE_KP_PLUS) && keyboard_state[SDL_SCANCODE_F])
        {
            increase_fog_density(NULL, scene);
        }
        else if ((key == SDL_SCANCODE_MINUS || key == SDL_SCANCODE_KP_MINUS) && keyboard_state[SDL_SCANCODE_F])
        {
            decrease_fog_density(NULL, scene);
        }
        else
        {
            // Other key handling here, e.g.:
            switch (key)
            {
            case SDL_SCANCODE_SPACE:
                handle_jump(scene, true);
                break;
            case SDL_SCANCODE_R:
                pre_restart(scene, scene->camera, scene->window_width, scene->window_height);
                post_restart(scene);
                break;
            case SDL_SCANCODE_H:
                toggle_help(NULL, scene);
                break;
            case SDL_SCANCODE_ESCAPE:
                exit_game(NULL, scene);
                break;
            default:
                break;
            }
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
