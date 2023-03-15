#ifndef SHADERMANAGER_H_
#define SHADERMANAGER_H_

#include <memory>
#include <map>
#include <string>

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

using namespace std;

class ShaderManager {

public:
   static ShaderManager& instance();
   ~ShaderManager();

   void initialize();
   void installShader(string name, GLhandleARB handle);
   void setActiveShader(string name);
   GLhandleARB getShader(string name);
   GLint getUniformLocation(const char *shaderName, const GLchar *varName);
   GLint getAttribLocation(const GLchar *varName);

private:
   ShaderManager();
   void printShaderInfoLog(GLhandleARB shader);

   static auto_ptr<ShaderManager> s_instance;
   map<string, GLhandleARB> shaders;
   string currentShader;
};

#endif /*SHADERMANAGER_H_*/
