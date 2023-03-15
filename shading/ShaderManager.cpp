#include "ShaderManager.h"
#include "Shader.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

std::auto_ptr<ShaderManager> ShaderManager::s_instance;

ShaderManager& ShaderManager::instance() {
   if(s_instance.get() == 0) {
      s_instance.reset(new ShaderManager);
   }
   return *s_instance;
}

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
   map<string, GLhandleARB>::iterator iter;
   for(iter = shaders.begin(); iter != shaders.end(); ++iter ) {
      glDeleteObjectARB(iter->second);
   }
}

void ShaderManager::initialize() {
   DIR *dp = opendir("shaders");
   if(dp == NULL) {
      printf("Unable to open shaders directory.\n");
      exit(1);
   }

   struct dirent *f;
   while((f = readdir(dp))) {
      string file(f->d_name);
      size_t pos = file.find(".vert");

      if(pos != string::npos) {
         file = file.substr(0, pos);
         printf("Installing shader %s\n", file.c_str());
         string vertFile = "shaders/" + file + ".vert";
         string fragFile = "shaders/" + file + ".frag";

         Shader shader;
         shader.installShader(vertFile.c_str(), GL_VERTEX_SHADER);
         shader.installShader(fragFile.c_str(), GL_FRAGMENT_SHADER);
         installShader(file, shader.linkProgram());
      }
   }
   closedir(dp);
}

void ShaderManager::installShader(string name, GLhandleARB handle) {
   shaders.insert(pair<string, GLhandleARB> (name, handle));
}

void ShaderManager::setActiveShader(string name) {
   currentShader = name;
   if(name == "none" || shaders.find(name) == shaders.end()) {
      glUseProgramObjectARB(0);
   }
   else {
      glUseProgramObjectARB(shaders[name]);
      printShaderInfoLog(shaders[name]);
   }
}

GLhandleARB ShaderManager::getShader(string name) {
   return shaders[name];
}

void ShaderManager::printShaderInfoLog(GLhandleARB shader) {
   GLint infologLength = 0;
   GLint charsWritten = 0;
   GLchar *infoLog;

   glGetObjectParameterivARB(shader, GL_INFO_LOG_LENGTH, &infologLength);

   if (infologLength > 1) {
      fprintf(stderr, "Log Length = %d\n", infologLength);

      infoLog = (GLchar *)malloc(infologLength);
      if (infoLog == NULL) {
         fprintf(stderr, "ERROR: Could not allocate InfoLog buffer\n");
         return;
      }

      glGetInfoLogARB(shader, infologLength, &charsWritten, infoLog);
      fprintf(stderr, "Shader InfoLog:%s\n\n", infoLog);
      free(infoLog);
   }
}

GLint ShaderManager::getUniformLocation(const char *shaderName, const GLchar *varName) {
   GLhandleARB handle = shaders[shaderName];
   GLint loc = glGetUniformLocationARB(handle, varName);
   if (loc == -1) {
      fprintf(stderr, "No such uniform named \"%s\"\n", varName);
   }
   return loc;
}

GLint ShaderManager::getAttribLocation(const GLchar *varName) {
   GLhandleARB handle = shaders["normal_map"];
   GLint loc = glGetAttribLocationARB(handle, varName);
   if (loc == -1) {
      fprintf(stderr, "No such attribute named \"%s\"\n", varName);
   }
   return loc;
}
