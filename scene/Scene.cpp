#include "Scene.h"
#include "Object.h"
#include "shading/ShaderManager.h"
#include "geometry/MeshManager.h"
#include "geometry/parsers/M3DSParser.h"
#include "Lights.h"
#include "utility/Texture.h"
#include <fstream>
#include <SDL/SDL_image.h>
#include <GL/glu.h>

Scene::Scene(const string setupFile) : camera(), stopApp(false),
   delU(0), delV(0), delN(0),
   roll(0), pitch(0), yaw(0)
{
   ifstream fp(setupFile.c_str());
   string str((istreambuf_iterator<char>(fp)), istreambuf_iterator<char>());
   fp.close();

   JSONNode config = libjson::parse(str.c_str());

   slideVel = pow(2.0, 2.0) / 100.0;
   rollVel = pow(3.0, 2.0) / 100.0;
   axis = false;
   skybox = false;

   setup();
   loadScene(config);
}

Scene::~Scene() {
   for(vector<Object*>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
      delete *it;
   }
   objects.clear();
   
   if(skybox) {
      delete[] skyTex;
   }

   IMG_Quit();
   SDL_Quit();
}

void Scene::setup() {
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   //Depth states
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);

   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CCW);
   glShadeModel(GL_SMOOTH);

   glEnable(GL_LIGHTING);

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::loadScene(const JSONNode& config) {
   JSONNode array = config.find("objects")->as_array();
   for(JSONNode::const_iterator it = array.begin(); it != array.end(); ++it) {
      objects.push_back(Object::load(*it));
   }

   if(config.find("axis") != config.end()) {
      axis = true;
   }
   if(config.find("skybox") != config.end()) {
      skySize = config.find("skybox")->as_float();
      skybox = true;
      skyTex = new GLuint[6];
      skyTex[0] = loadTexture("resources/images/stars/stars0000.png", GL_REPLACE);
      skyTex[1] = loadTexture("resources/images/stars/stars0001.png", GL_REPLACE);
      skyTex[2] = loadTexture("resources/images/stars/stars0002.png", GL_REPLACE);
      skyTex[3] = loadTexture("resources/images/stars/stars0003.png", GL_REPLACE);
      skyTex[4] = loadTexture("resources/images/stars/stars0004.png", GL_REPLACE);
      skyTex[5] = loadTexture("resources/images/stars/stars0005.png", GL_REPLACE);
   }

   JSONNode eye = config.find("eye")->as_array();
   Point3D cameraEye(eye.at(0).as_float(), eye.at(1).as_float(), eye.at(2).as_float());

   JSONNode::const_iterator it = config.find("lookAt");
   if(it != config.end()) {
      JSONNode l = it->as_array();
      Point3D lookAt(l.at(0).as_float(), l.at(1).as_float(), l.at(2).as_float());
      camera.set(cameraEye, lookAt, Vector3D(0, 1, 0));
   }
   else {
      JSONNode rotate = config.find("rotate")->as_array();
      Point3D lookAt(cameraEye.x, cameraEye.y, cameraEye.z - 1);
      camera.set(cameraEye, lookAt, Vector3D(0, 1, 0));
      camera.roll(-rotate.at(2).as_float());
      camera.pitch(-rotate.at(0).as_float());
      camera.yaw(-rotate.at(1).as_float());
   }
}

void Scene::start() {
   SDL_Event event;
   while(!stopApp) {
      while ( SDL_PollEvent(&event)) {
         switch (event.type) {
            case SDL_KEYDOWN :
               handleKeydown(event.key);
               break;
            case SDL_KEYUP :
               handleKeyup(event.key);
               break;
            case SDL_QUIT:
               stopApp = true;
               break;
         }
      }

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      draw();
      glFlush();
      SDL_GL_SwapBuffers();
   }
}

void Scene::draw() {
   glPushMatrix();

   camera.roll(roll);
   camera.pitch(pitch);
   camera.yaw(yaw);
   
   if(skybox) {
      drawSkybox();
   }
   
   camera.slide(delU, delV, delN);
   camera.loadGLMatrix();

   Lights::instance().applyLights();

   if(axis) {
      drawAxis();
   }

   for(vector<Object*>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
      (*it)->draw();
   }

   glPopMatrix();
}

void Scene::drawAxis() {
   ShaderManager::instance().setActiveShader("none");
   glDisable(GL_LIGHTING);
   
   glBegin(GL_LINES);
   glColor3f(1, 0, 0);
   glVertex3i(0, 0, 0);
   glVertex3i(10, 0, 0);
   
   glColor3f(0, 1, 0);
   glVertex3i(0, 0, 0);
   glVertex3i(0, 10, 0);
   
   glColor3f(0, 0, 1);
   glVertex3i(0, 0, 0);
   glVertex3i(0, 0, 10);
   glEnd();
   
   glEnable(GL_LIGHTING);
}

void Scene::drawSkybox() {
   glPushMatrix();
   glLoadIdentity();
   camera.m.m14() = 0;
   camera.m.m24() = 0;
   camera.m.m34() = 0;
   camera.loadGLMatrix();
   
   glPushAttrib(GL_ENABLE_BIT);
   glDisable(GL_LIGHTING);
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);
   
   // Render the front quad
   glBindTexture(GL_TEXTURE_2D, skyTex[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3f( -skySize, -skySize, -skySize );
   glTexCoord2f(1, 0); glVertex3f(  skySize, -skySize, -skySize );
   glTexCoord2f(1, 1); glVertex3f(  skySize,  skySize, -skySize );
   glTexCoord2f(0, 1); glVertex3f( -skySize,  skySize, -skySize );
   glEnd();
   
   // Render the left quad
   glBindTexture(GL_TEXTURE_2D, skyTex[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3f( -skySize, -skySize,  skySize );
   glTexCoord2f(1, 0); glVertex3f( -skySize, -skySize, -skySize );
   glTexCoord2f(1, 1); glVertex3f( -skySize,  skySize, -skySize );
   glTexCoord2f(0, 1); glVertex3f( -skySize,  skySize,  skySize );
   glEnd();
   
   // Render the back quad
   glBindTexture(GL_TEXTURE_2D, skyTex[2]);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3f(  skySize, -skySize,  skySize );
   glTexCoord2f(1, 0); glVertex3f( -skySize, -skySize,  skySize );
   glTexCoord2f(1, 1); glVertex3f( -skySize,  skySize,  skySize );
   glTexCoord2f(0, 1); glVertex3f(  skySize,  skySize,  skySize );
   glEnd();
   
   // Render the right quad
   glBindTexture(GL_TEXTURE_2D, skyTex[3]);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3f(  skySize, -skySize, -skySize );
   glTexCoord2f(1, 0); glVertex3f(  skySize, -skySize,  skySize );
   glTexCoord2f(1, 1); glVertex3f(  skySize,  skySize,  skySize );
   glTexCoord2f(0, 1); glVertex3f(  skySize,  skySize, -skySize );
   glEnd();
   
   // Render the top quad
   glBindTexture(GL_TEXTURE_2D, skyTex[4]);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 1); glVertex3f( -skySize,  skySize,  skySize );
   glTexCoord2f(0, 0); glVertex3f( -skySize,  skySize, -skySize );
   glTexCoord2f(1, 0); glVertex3f(  skySize,  skySize, -skySize );
   glTexCoord2f(1, 1); glVertex3f(  skySize,  skySize,  skySize );
   glEnd();
   
   // Render the bottom quad
   glBindTexture(GL_TEXTURE_2D, skyTex[5]);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0); glVertex3f( -skySize, -skySize, -skySize );
   glTexCoord2f(0, 1); glVertex3f( -skySize, -skySize,  skySize );
   glTexCoord2f(1, 1); glVertex3f(  skySize, -skySize,  skySize );
   glTexCoord2f(1, 0); glVertex3f(  skySize, -skySize, -skySize );
   glEnd();

   glPopAttrib();
   glPopMatrix();
}

void Scene::handleKeydown(SDL_KeyboardEvent &event) {
   switch(event.keysym.sym) {
      case 'w' :
         if(event.keysym.mod & KMOD_LCTRL) {
            delV = slideVel;
         } else {
            delN = -slideVel;
         }
         break;
      case 's' :
         if(event.keysym.mod & KMOD_LCTRL) {
            delV = -slideVel;
         } else {
            delN = slideVel;
         }
         break;
      case 'a' :
         if(event.keysym.mod & KMOD_LCTRL) {
            yaw = -rollVel;
         } else {
            delU = -slideVel;
         }
         break;
      case 'd' :
         if(event.keysym.mod & KMOD_LCTRL) {
            yaw = rollVel;
         } else {
            delU = slideVel;
         }
         break;
      case SDLK_LEFT:
         roll = rollVel;
         break;
      case SDLK_RIGHT:
         roll = -rollVel;
         break;
      case SDLK_UP:
         pitch = -rollVel;
         break;
      case SDLK_DOWN:
         pitch = rollVel;
         break;
      default: break;
   }
}

void Scene::handleKeyup(SDL_KeyboardEvent &event) {
   if(event.keysym.sym >= 49 && event.keysym.sym <= 57) {
      slideVel = pow(2.0, (event.keysym.sym - 48)) / 100.0;
      rollVel = pow(2.8, (event.keysym.sym - 48)) / 100.0;
   }
   else {
      switch(event.keysym.sym) {
         case SDLK_ESCAPE :
            stopApp = true;
            break;
         case '0':
            slideVel = pow(2.0, 10) / 10.0;
            break;
         case 'w' :
         case 's' :
            delV = delN = 0;
            break;
         case 'a' :
         case 'd' :
            yaw = delU = 0;
            break;
         case SDLK_LEFT:
         case SDLK_RIGHT:
            roll = 0;
            break;
         case SDLK_UP:
         case SDLK_DOWN:
            pitch = 0;
            break;
         default: break;
      }
   }
}