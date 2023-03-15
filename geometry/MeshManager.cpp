#include <fstream>
#include "MeshManager.h"
#include "Mesh.h"
#include "GeoSphere.h"
#include "parsers/M3DSParser.h"
#include "parsers/LightWaveParser.h"

auto_ptr<MeshManager> MeshManager::s_instance;

MeshManager& MeshManager::instance() {
   if(s_instance.get() == 0) {
      s_instance.reset(new MeshManager());
   }
   return *s_instance;
}

MeshManager::MeshManager() {
}

MeshManager::~MeshManager() {
   objects.clear();
}

void MeshManager::loadMeshes(string fname) {
   ifstream fp(fname.c_str());
   string str((istreambuf_iterator<char>(fp)), istreambuf_iterator<char>());
   fp.close();

   JSONNode root = libjson::parse(str.c_str());
   JSONNode array = root.at(0);

   for(JSONNode::const_iterator it = array.begin(); it != array.end(); ++it) {
      loadMesh(*it);
   }

   GeoSphere sphere15(15);
   vector<GLuint> sphereVec;
   sphereVec.push_back(sphere15.createDisplayList());
   objects["sphere15"] = sphereVec;
}

void MeshManager::loadMesh(const JSONNode& node) {
   string type = node.find("type")->as_string();
   string name = node.find("name")->as_string();
   string path = node.find("path")->as_string();
   string file = node.find("file")->as_string();

   if(type == "m3ds") {
      M3DSParser parser(path);
      objects[name] = parser.load(file);
   }
   else if(type == "lightwave") {
      LightWaveParser parser(path);
      objects[name] = parser.load(file);
   }
}
