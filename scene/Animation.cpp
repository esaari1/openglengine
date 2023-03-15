#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include "Animation.h"
#include "Lights.h"
#include "Object.h"
#include <fstream>

typedef JSONNode::const_iterator NodeIter;

void FrameObject::draw() {
   obj->setPosition(position);
   obj->setRotation(rotation);
   obj->draw();
}

Animation::Animation(const string setupFile, const string animationFile) : Scene(setupFile), outputDir(), cameraFrames(), objFrames() {
   loadAnimation(animationFile);
}

Animation::~Animation() {
}

void Animation::start() {
   Vector3D up(0, 1, 0);

   for(size_t i = 0; i < cameraFrames.size(); i++) {
      const Point3D& eye = cameraFrames[i]->position;

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glPushMatrix();
      glLoadIdentity();

      camera.reset();
      camera.set(eye, Point3D(eye.x, eye.y, eye.z - 1), up);
      camera.roll(-cameraFrames[i]->rotation.z);
      camera.pitch(-cameraFrames[i]->rotation.x);
      camera.yaw(-cameraFrames[i]->rotation.y);
      
      if(skybox) {
         drawSkybox();
         camera.setPosition(eye);
      }

      camera.loadGLMatrix();

      Lights::instance().applyLights();

      for(vector<Object*>::const_iterator it = objects.begin(); it != objects.end(); ++it) {
         (*it)->draw();
      }

      for(vector<FrameObject*>::const_iterator it2 = objFrames[i]->objects.begin(); it2 != objFrames[i]->objects.end(); ++it2) {
         (*it2)->draw();
      }

      glPopMatrix();

      glFlush();
      SDL_GL_SwapBuffers();
   }
}

void Animation::loadAnimation(string fname) {
   ifstream fp(fname.c_str());
   string str((istreambuf_iterator<char>(fp)), istreambuf_iterator<char>());
   fp.close();

   JSONNode root = libjson::parse(str.c_str());
   outputDir = root.find("outputDir")->as_string();

   JSONNode array = root.find("objects")->as_array();
   for(NodeIter it = array.begin(); it != array.end(); ++it) {
      string name = it->find("name")->as_string();
      if(name == "Camera") {
         loadCameraFrames(it->find("frames")->as_array());
      }
      else {
         for(vector<Object*>::iterator obj = objects.begin(); obj != objects.end(); ++obj) {
            if((*obj)->getName() == name) {
               loadAnimationFrames(it->find("frames")->as_array(), *obj);
               objects.erase(obj);
               break;
            }
         }
      }
   }
}

void Animation::loadCameraFrames(const JSONNode& frames) {
   for(NodeIter it = frames.begin(); it != frames.end(); ++it) {
      CameraFrame* cf = new CameraFrame();
      JSONNode pos = it->find("position")->as_array();
      cf->position.set(pos.at(0).as_float(), pos.at(1).as_float(), pos.at(2).as_float());

      JSONNode rot = it->find("rotation")->as_array();
      cf->rotation.set(rot.at(0).as_float(), rot.at(1).as_float(), rot.at(2).as_float());
      cameraFrames.push_back(cf);
   }
}

void Animation::loadAnimationFrames(const JSONNode& frames, Object* obj) {
   if(objFrames.empty()) {
      objFrames.reserve(frames.size());
      for(unsigned i = 0; i < frames.size(); i++) {
         objFrames.push_back(new Frame());
      }
   }

   int i = 0;
   for(NodeIter it = frames.begin(); it != frames.end(); ++it) {
      FrameObject* fobj = new FrameObject(obj);
      JSONNode pos = it->find("position")->as_array();
      fobj->position.set(pos.at(0).as_float(), pos.at(1).as_float(), pos.at(2).as_float());

      JSONNode rot = it->find("rotation")->as_array();
      fobj->rotation.set(rot.at(0).as_float(), rot.at(1).as_float(), rot.at(2).as_float());
      objFrames[i++]->objects.push_back(fobj);
   }
}
