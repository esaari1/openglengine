#include "Lights.h"
#include <fstream>
#include "math/Vector3D.h"
#include "math/Matrix.h"

using namespace std;

auto_ptr<Lights> Lights::s_instance;

Lights& Lights::instance() {
   if(s_instance.get() == 0) {
      s_instance.reset(new Lights());
   }
   return *s_instance;
}

Lights::Lights() : lightCount(0) {
}

Lights::~Lights() {
   
}

void Lights::loadLights(string fname) {
   ifstream fp(fname.c_str());
   string str((istreambuf_iterator<char>(fp)), istreambuf_iterator<char>());
   fp.close();

   JSONNode root = libjson::parse(str.c_str());
   JSONNode lights = root.find("lights")->as_array();

   int lidx = 0;
   lightCount = lights.size();
   positions = new GLfloat*[lightCount];

   for(JSONNode::const_iterator it = lights.begin(); it != lights.end(); ++it) {
      glEnable(GL_LIGHT0 + lidx);

      JSONNode node = it->find("diffuse")->as_array();
      GLfloat diffuse_light[] = { node.at(0).as_float(), node.at(1).as_float(), node.at(2).as_float(), node.at(3).as_float() };
      glLightfv(GL_LIGHT0 + lidx, GL_DIFFUSE, diffuse_light);

      if(it->find("position") != it->end()) {
         node = it->find("position")->as_array();
         positions[lidx] = new GLfloat[4];
         for(int i = 0; i < 4; i++) {
            positions[lidx][i] = node.at(i).as_float();
         }
      }
      else {
         node = it->find("rotation")->as_array();
         setRotation(node, lidx);
      }

      lidx++;
   }

   JSONNode amb = root.find("ambient")->as_array();
   GLfloat ambient_light[] = { amb.at(0).as_float(), amb.at(1).as_float(), amb.at(2).as_float(), amb.at(3).as_float() };
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
}

void Lights::applyLights() {
   for(int lidx = 0; lidx < lightCount; lidx++) {
      glLightfv(GL_LIGHT0 + lidx, GL_POSITION, positions[lidx]);
   }
}

void Lights::setRotation(const JSONNode& node, int lidx) {
   Vector3D dir(0, 0, 1);
   Matrix m;
   m.rotate(node.at(0).as_float(), 1, 0, 0);
   m.rotate(node.at(1).as_float(), 0, 1, 0);
   m.rotate(node.at(2).as_float(), 0, 0, 1);
   dir = m * dir;

   positions[lidx] = new GLfloat[4];
   positions[lidx][0] = dir.x;
   positions[lidx][1] = dir.y;
   positions[lidx][2] = dir.z;
   positions[lidx][3] = 0;
}
