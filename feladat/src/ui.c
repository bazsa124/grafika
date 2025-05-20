#include "ui.h"

UI_Element* new_ui_element()
{
    UI_Element* element = malloc(sizeof(UI_Element));
    if (!element) {
        fprintf(stderr, "Failed to allocate memory for UI element\n");
        exit(1);
    }

    return element;
}

void init_ui_element(UI_Element* element, vec3 position, vec3 size, GLuint texture_id)
{
    element->position   = position;
    element->size       = size;
    element->texture_id = texture_id;
    
    element->is_visible = true;

    element->anchor_x = UI_ANCHOR_CENTER;
    element->anchor_y = UI_ANCHOR_CENTER;

    element->on_mouse_click = NULL;
}

void set_ui_element_mouse_click(UI_Element* element, OnMouseClick on_mouse_click)
{
    element->on_mouse_click = on_mouse_click;
}

void on_click(UI_Element* element, Scene* scene)
{
    if (element->on_mouse_click) {
        element->on_mouse_click(element, scene);
    }
}

float calculate_real_pos(UI_Anchor_Type anchor, float base_pos, float base_size, int window_size) {
    float result = base_pos;

    switch (anchor)
    {
    case UI_ANCHOR_CENTER:
        result += (window_size - base_size) / 2;
        break;
    case UI_ANCHOR_CLOSE:
        // Do nothing.
        break;
    case UI_ANCHOR_FAR:
        result = window_size - base_pos - base_size;
        break;
    }

    return result;
}

bool is_mouse_over_ui_element(UI_Element* element, vec3 mouse_position, int window_width, int window_height)
{
    float position_x = calculate_real_pos(element->anchor_x, element->position.x, element->size.x, window_width);
    float position_y = calculate_real_pos(element->anchor_y, element->position.y, element->size.y, window_height);

    return mouse_position.x >= position_x && mouse_position.x <= position_x + element->size.x &&
           mouse_position.y >= position_y && mouse_position.y <= position_y + element->size.y;
}

void render_ui_element(UI_Element* element, int window_width, int window_height)
{
    float position_x = calculate_real_pos(element->anchor_x, element->position.x, element->size.x, window_width);
    float position_y = calculate_real_pos(element->anchor_y, element->position.y, element->size.y, window_height);

    if (element->is_visible) {
        glBindTexture(GL_TEXTURE_2D, element->texture_id);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(position_x                  , position_y                  );
        glTexCoord2f(1.0f, 0.0f); glVertex2f(position_x + element->size.x, position_y                  );
        glTexCoord2f(1.0f, 1.0f); glVertex2f(position_x + element->size.x, position_y + element->size.y);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(position_x                  , position_y + element->size.y);
        glEnd();
    }
}

void set_2d_render_mode(int window_width, int window_height)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, window_height, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void reset_render_mode()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);
}