#ifndef SHADER_HPP_
#define SHADER_HPP_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

class Shader {

public:
   Shader();
   ~Shader();

   int installShader(const char *filename, GLenum shaderType);
   GLhandleARB linkProgram();
   GLint getUniformLocation(const GLchar *name);
   void bindAttribLocation(const GLuint index, const char *name);

private:
   GLhandleARB programHandle;

   void printShaderInfoLog(GLhandleARB shader);
   int readShader(const char *fileName, GLchar **shaderText);
   long shaderSize(const char *fileName);
};

#endif /*SHADER_HPP_*/
