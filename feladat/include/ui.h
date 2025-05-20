#ifndef UI_H
#define UI_H

#include "utils.h"

#include <GL/gl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum UI_Anchor_Type
{
    UI_ANCHOR_CENTER,
    UI_ANCHOR_CLOSE,
    UI_ANCHOR_FAR,
} UI_Anchor_Type;

typedef struct UI_Element UI_Element;
typedef struct Scene Scene;

typedef void (*OnMouseClick)(UI_Element* element, Scene* scene);

struct UI_Element
{
    vec3 position;
    vec3 size;
    GLuint texture_id;
    bool is_visible;

    UI_Anchor_Type anchor_x;
    UI_Anchor_Type anchor_y;

    OnMouseClick on_mouse_click;
};

/**
 * Allocate a new UI element.
 */
UI_Element* new_ui_element();

/**
 * Initialize the UI element.
 */
void init_ui_element(UI_Element* element, vec3 position, vec3 size, GLuint texture_id);

/**
 * Set the UI element's mouse click callback.
 */
void set_ui_element_mouse_click(UI_Element* element, OnMouseClick on_mouse_click);

/**
 * On click callback.
 */
void on_click(UI_Element* element, Scene* scene);

/**
 * Check if the mouse is over the UI element.
 */
bool is_mouse_over_ui_element(UI_Element* element, vec3 mouse_position, int window_width, int window_height);

/**
 * Render the UI element.
 */
void render_ui_element(UI_Element* element, int window_width, int window_height);

/**
 * 2D render mode.
 */
void set_2d_render_mode(int window_width, int window_height);

/**
 * Reset render mode.
 */
void reset_render_mode();

#endif /* UI_H */