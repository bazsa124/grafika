#ifndef APP_H
#define APP_H

#include "camera.h"
#include "scene.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

typedef struct App
{
    SDL_Window *window;
    SDL_GLContext gl_context;
    bool is_running;
    bool is_restart;
    double uptime;
    Camera camera;
    Scene scene;
    int window_width;
    int window_height;
} App;

/**
 * Initialize the application.
 */
void init_app(App *app, int argc, char *argv[]);

/**
 * Initialize the OpenGL context.
 */
void init_opengl();

/**
 * Reshape the window.
 */
void reshape(GLsizei width, GLsizei height);

/**
 * Handle the events of the application.
 */
void handle_app_events(App *app);

/**
 * Update the application.
 */
void update_app(App *app);

/**
 * Render the application.
 */
void render_app(App *app);

/**
 * Destroy the application.
 */
void destroy_app(App *app);

#endif /* APP_H */
