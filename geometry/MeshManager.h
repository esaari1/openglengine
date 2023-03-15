#ifndef _MESH_MANAGER_
#define _MESH_MANAGER_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "libjson/libjson.h"

using namespace std;

class Mesh;

typedef map<string, vector<GLuint> >::const_iterator MeshIter;

class MeshManager {

public:
   static MeshManager& instance();
   ~MeshManager();

   void loadMeshes(string fname);
   vector<GLuint> getMesh(string name) { return objects[name]; }

private:
   MeshManager();
   void loadMesh(const JSONNode& node);

   static auto_ptr<MeshManager> s_instance;
   map<string, vector<GLuint> > objects;
};

#endif
