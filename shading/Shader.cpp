#include "Shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void Shader::printShaderInfoLog(GLhandleARB shader) {
   GLint infologLength = 0;
   GLint charsWritten = 0;
   GLchar *infoLog;

   glGetObjectParameterivARB(shader, GL_INFO_LOG_LENGTH, &infologLength);
   fprintf(stderr, "%d\n", infologLength);

   if (infologLength > 0) {
      infoLog = (GLchar *)malloc(infologLength);
      if (infoLog == NULL) {
         fprintf(stderr, "ERROR: Could not allocate InfoLog buffer\n");
         return;
      }
      
      glGetInfoLogARB(shader, infologLength, &charsWritten, infoLog);
      fprintf(stderr, "Shader InfoLog:\n%s\n\n", infoLog);
      free(infoLog);
   }
}

// Returns the size in bytes of the shader fileName.
// If an error occurred, it returns -1.
long Shader::shaderSize(const char *fileName) {
   int fd;
   long count = -1;

#ifdef WIN32 /*[*/
   fd = _open(fileName, _O_RDONLY);
   if (fd != -1) {
      count = _lseek(fd, 0, SEEK_END) + 1;
      _close(fd);
   }
#else /*][*/
   fd = open(fileName, O_RDONLY);
   if (fd != -1)
   {
      count = lseek(fd, 0, SEEK_END) + 1;
      close(fd);
   }
#endif /*]*/

   return count;
}

Shader::Shader() {
   programHandle = glCreateProgramObjectARB();
}

Shader::~Shader() {
}

int Shader::readShader(const char *fileName, GLchar **shaderText) {
   long size = shaderSize(fileName);
   if(size < 0) {
      fprintf(stderr, "Cannot determine size of the shader %s\n", fileName);
      return -1;
   }
   
   *shaderText = (GLchar *) malloc(size);

   FILE *fh = fopen(fileName, "r");
   if (!fh) {
      return -1;
   }

   // Get the shader from a file.
   fseek(fh, 0, SEEK_SET);
   int count = (int) fread(*shaderText, 1, size, fh);
   (*shaderText)[count] = '\0';

   if (ferror(fh)) {
      count = 0;
   }

   fclose(fh);
   return count;
}

int Shader::installShader(const char *filename, GLenum shaderType) {
   GLchar *shaderText;
   GLint status;
   readShader(filename, &shaderText);

   GLhandleARB handle = glCreateShaderObjectARB(shaderType);
   glShaderSourceARB(handle, 1, (const GLchar **) &shaderText, NULL);
   free(shaderText);
   
   glCompileShaderARB(handle);
   glGetObjectParameterivARB(handle, GL_COMPILE_STATUS, &status);
   if(status == GL_FALSE) {
      fprintf(stderr, "Error compiling shader %s\n", filename);
      printShaderInfoLog(handle);
      return 0;
   }
   
   glAttachObjectARB(programHandle, handle);
   
   // Mark handle for deletion. It will not be deleted until it is
   // detached from the program.
   glDeleteObjectARB(handle);
   
   printf("Shader %s successfully installed\n", filename);
   return 0;
}

GLhandleARB Shader::linkProgram() {
   glLinkProgramARB(programHandle);
   
   GLint status;
   glGetObjectParameterivARB(programHandle, GL_LINK_STATUS, &status);

   if(status == GL_FALSE) {
      fprintf(stderr, "Error linking shaders, status = %d\n", status);
      printShaderInfoLog(programHandle);
      return 0;
   }
   
   glGetObjectParameterivARB(programHandle, GL_OBJECT_LINK_STATUS_ARB, &status);

   if(status == GL_FALSE) {
      fprintf(stderr, "Error linking shaders, status = %d\n", status);
      printShaderInfoLog(programHandle);
      return 0;
   }

   glUseProgramObjectARB(programHandle);
   return programHandle;
}

GLint Shader::getUniformLocation(const GLchar *name) {
   GLint loc = glGetUniformLocationARB(programHandle, name);
   if (loc == -1) {
      fprintf(stderr, "No such uniform named \"%s\"\n", name);
   }
   return loc;
}

void Shader::bindAttribLocation(const GLuint index, const char *name) {
   glBindAttribLocationARB(programHandle, index, name);
}
