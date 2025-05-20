#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

/**
 * Load texture from file and returns with the texture name.
 */
GLuint load_texture(const char* filename);

#endif /* TEXTURE_H */
