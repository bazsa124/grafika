#include "app.h"

#include <SDL2/SDL_image.h>

#include <math.h>

void init_app(App *app, int argc, char *argv[])
{
    // Suppress unused parameter warnings.
    (void)argc;

    int error_code;
    int inited_loaders;

    app->is_running = false;
    app->is_restart = false;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0)
    {
        printf("[ERROR] SDL initialization error: %s\n", SDL_GetError());
        return;
    }
    printf("[INFO] SDL initialized.\n");

    // Get the monitor index from the command line arguments.
    char *end;
    long monitor_index = strtol(argv[1], &end, 10);
    if (end == argv[1] || *end != '\0' /*|| errno == ERANGE*/)
    {
        printf("[INFO] No/Invalid monitor index provided. Defaulting to 0.\n");
        monitor_index = 0;
    }

    SDL_DisplayMode display_mode;
    error_code = SDL_GetCurrentDisplayMode(monitor_index, &display_mode);
    if (error_code != 0)
    {
        printf("[ERROR] Unable to get the display mode: %s\n", SDL_GetError());
        return;
    }
    printf("[INFO] Display mode: %dx%d\n", display_mode.w, display_mode.h);

    app->window_width = display_mode.w;
    app->window_height = display_mode.h;

    app->window = SDL_CreateWindow(
        "Cube!",
        SDL_WINDOWPOS_CENTERED_DISPLAY(monitor_index), SDL_WINDOWPOS_CENTERED,
        app->window_width, app->window_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (app->window == NULL)
    {
        printf("[ERROR] Unable to create the application window!\n");
        return;
    }
    printf("[INFO] Window created.\n");

    inited_loaders = IMG_Init(IMG_INIT_PNG);
    if (inited_loaders == 0)
    {
        printf("[ERROR] IMG initialization error: %s\n", IMG_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL)
    {
        printf("[ERROR] Unable to create the OpenGL context!\n");
        return;
    }
    printf("[INFO] OpenGL context created.\n");

    init_opengl();
    reshape(app->window_width, app->window_height);

    init_camera(&(app->camera));
    init_scene(&(app->scene), &(app->camera), app->window_width, app->window_height);
    app->is_running = true;
}

void init_opengl()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.1, 0.1, 0.1, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

bool first = true;
float w_frustum;
float h_frustum;

void reshape(GLsizei width, GLsizei height)
{
    if (first)
    {
        first = false;
        float x = 0;
        float y = 0;
        float w = width;
        float h = height;
        glViewport(x, y, w, h);

        float fov = 75;

        w_frustum = tan(degree_to_radian(fov) / 2) * .3;
        h_frustum = w_frustum * (h / w);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(
        -w_frustum, w_frustum,
        -h_frustum, h_frustum,
        .1, 100);
}

void handle_app_events(App *app)
{
    SDL_Event event;
    static bool is_ctrl_pressed = false;
    static int mouse_x = 0;
    static int mouse_y = 0;
    int x;
    int y;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
                break;
            case SDL_SCANCODE_W:
                set_camera_speed(&(app->camera), 1);
                break;
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), -1);
                break;
            case SDL_SCANCODE_A:
                set_camera_side_speed(&(app->camera), 1);
                break;
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), -1);
                break;
            case SDL_SCANCODE_LCTRL:
                is_ctrl_pressed = true;
                break;
            case SDL_SCANCODE_R:
                app->is_restart = true;
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), 0);
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), 0);
                break;
            case SDL_SCANCODE_LCTRL:
                is_ctrl_pressed = false;
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&x, &y);
            if (!is_ctrl_pressed)
            {
                rotate_camera(&(app->camera), mouse_x - x, mouse_y - y);
            }

            mouse_x = x;
            mouse_y = y;
            break;
        case SDL_QUIT:
            app->is_running = false;
            break;
        default:
            break;
        }

        handle_scene_events(&(app->scene), &event);
    }
}

void update_app(App *app)
{
    double current_time;
    double elapsed_time;

    current_time = (double)SDL_GetTicks() / 1000;
    elapsed_time = current_time - app->uptime;

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene));
}

void render_app(App *app)
{
    double current_time;
    double elapsed_time;

    current_time = (double)SDL_GetTicks() / 1000.0;
    elapsed_time = (current_time - app->uptime);
    app->uptime = current_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    reshape(app->window_width, app->window_height);
    set_view(&(app->camera));
    render_scene(&(app->scene), (float)elapsed_time);
    glPopMatrix();

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App *app)
{
    if (app->gl_context != NULL)
    {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL)
    {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
}
