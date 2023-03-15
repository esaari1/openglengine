#ifndef _SCENE_H_
#define _SCENE_H_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include <SDL/SDL.h>
#include <string>
#include <vector>
#include "Camera.h"
#include "libjson/libjson.h"

using namespace std;

class Object;

class Scene {

public:
   Scene(const string setupFile);
   virtual ~Scene();

   virtual void start();

protected:
   void drawSkybox();

   Camera camera;
   vector<Object*> objects;
   bool skybox;

private:
   void setup();
   void loadScene(const JSONNode& config);

   void draw();
   void drawAxis();
   
   void handleKeydown(SDL_KeyboardEvent &event);
   void handleKeyup(SDL_KeyboardEvent &event);

   bool stopApp;
   float delU, delV, delN;
   float roll, pitch, yaw;
   float rollVel, slideVel;
   float skySize;
   bool axis;
   GLuint* skyTex;
};

#endif
