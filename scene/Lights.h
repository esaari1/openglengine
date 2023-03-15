#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include <memory>
#include <string>
#include "libjson/libjson.h"

using namespace std;

class Lights {

public:
   static Lights& instance();
   ~Lights();

   void loadLights(string fname);
   void applyLights();

private:
   Lights();
   void setRotation(const JSONNode& node, int lidx);
   
   static auto_ptr<Lights> s_instance;
   size_t lightCount;
   GLfloat** positions;
};

#endif
