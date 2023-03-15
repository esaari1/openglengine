#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "scene/Animation.h"
#include "scene/Lights.h"
#include "shading/ShaderManager.h"
#include "geometry/MeshManager.h"

void initSDL(const JSONNode& root);
void printControls();

void initSDL(const JSONNode& root) {
   int width = (int) root.find("width")->as_int();
   int height = (int) root.find("height")->as_int();

   SDL_Init(SDL_INIT_VIDEO);
   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
   SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 8 );
   SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
   SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
   SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_HWSURFACE | SDL_DOUBLEBUF); // | SDL_FULLSCREEN);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(49.134f , (float) width / height , root.find("near")->as_float() , root.find("far")->as_float());

   IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
}

void printControls() {
   printf("w - Move forward\n");
   printf("s - Move backward\n");
   printf("a - Move left\n");
   printf("d - Move right\n");
   printf("\n");
   printf("CTRL w - Move up\n");
   printf("CTRL s - Move down\n");
   printf("CTRL a - Rotate left\n");
   printf("CTRL d - Rotate right\n");
   printf("\n");
   printf("Left Arrow  - Spin left\n");
   printf("Right Arrow - Spin right\n");
   printf("Up Arrow    - Rotate up\n");
   printf("Down Arrow  - Rotate down\n");
   printf("\n");
   printf("0-9 - Set speed (slow to fast)\n");
}

int main(int argc, char **argv) {
   printControls();

   ifstream fp(argv[1]);
   string str((istreambuf_iterator<char>(fp)), istreambuf_iterator<char>());
   fp.close();

   JSONNode root = libjson::parse(str.c_str());
   initSDL(root);

#ifdef WIN32
   GLenum err = glewInit();
   if(err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      exit(err);
   }
#endif

   ShaderManager::instance().initialize();
   Lights::instance().loadLights(root.find("lights")->as_string());

   if(root.find("mesh") != root.end()) {
      MeshManager::instance().loadMeshes(root.find("mesh")->as_string());
   }

   string sceneFile = root.find("scene")->as_string();

   Scene* scene;
   if(root.find("animation") != root.end()) {
      scene = new Animation(sceneFile, root.find("animation")->as_string());
   }
   else {
      scene = new Scene(sceneFile);
   }

   scene->start();
   delete scene;

   return 0;
}
