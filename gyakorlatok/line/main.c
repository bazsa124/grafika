#include <stdio.h>

#include <GL/gl.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

int main(int argc, char *argv[])
{

    typedef struct
    {
        unsigned char r, g, b;
    } Color;

    typedef struct
    {
        float x, y;
    } Point;

    typedef struct
    {
        Point *start;
        Point *end;
        Color *color;
    } Line;

    Line l;
    Point start = {0.0, 0.0};
    Point end = {3.0, 3.0};
    Color color = {255.0, 0, 0};

    l.start = &start;
    l.end = &end;
    l.color = &color;

    printf("%lf",l.end->x);

    int error_code;
    SDL_Window *window;
    bool need_run;
    SDL_Event event;
    SDL_GLContext gl_context;
    int i;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0)
    {
        printf("[ERROR] SDL initialization error: %s\n", SDL_GetError());
        return error_code;
    }

    window = SDL_CreateWindow(
        "Hello SDL!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 10.0, 0.0, 10.0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);

    need_run = true;
    while (need_run)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            case SDL_SCANCODE_Q:
                need_run = false;
                break;
            case SDL_QUIT:
                need_run = false;
                break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3ub(l.color->r, l.color->g, l.color->b);

        glBegin(GL_LINES);
            glVertex2f(l.start->x, l.start->y);
            glVertex2f(l.end->x, l.end->y);
        glEnd();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}