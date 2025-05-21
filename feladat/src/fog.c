#include "fog.h"

void init_fog()
{
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogf(GL_FOG_DENSITY, 0.1f);
    glHint(GL_FOG_HINT, GL_NICEST);
}

void set_fog_density(GLfloat density)
{
    glFogf(GL_FOG_DENSITY, density);
}

void set_fog_color(vec3 color)
{
    float color_arr[] = { color.x, color.y, color.z, 1.0f };
    glFogfv(GL_FOG_COLOR, color_arr);
}