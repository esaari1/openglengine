#ifndef GLRenderer_Texture_h
#define GLRenderer_Texture_h

#include <string>

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

using namespace std;

GLuint loadTexture(string fname, GLint envMode = GL_MODULATE);

#endif
