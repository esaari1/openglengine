#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Scene.h"
#include "libjson/libjson.h"
#include <vector>
#include <string>

using namespace std;

struct CameraFrame {
   Point3D position;
   Vector3D rotation;
};

struct FrameObject {
   FrameObject(Object* o) : obj(o) {}
   void draw();

   Object* obj;
   Point3D position;
   Vector3D rotation;
};

struct Frame {
   vector<FrameObject*> objects;
};

class Animation : public Scene {

public:
   Animation(const string setupFile, const string animationFile);
   virtual ~Animation();

   virtual void start();

private:
   void loadAnimation(string fname);
   void loadCameraFrames(const JSONNode& frames);
   void loadAnimationFrames(const JSONNode& frames, Object* obj);

   string outputDir;
   vector<CameraFrame*> cameraFrames;
   vector<Frame*> objFrames;
};

#endif
