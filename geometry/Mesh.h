#ifndef _MESH_H_
#define _MESH_H_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include <string>
#include <vector>
#include <map>
#include "math/Vector3D.h"

using namespace std;

class Point2D;
class Point3D;
class Vector3D;
class Material;
class Mesh;

class Face {

public:
   Face(Mesh* parent, int idx1, int idx2, int idx3);
   ~Face() { }

   void setTextureIdxs(int idx1, int idx2, int idx3);
   bool hasTextureCoords() const { return hasTexCoords; }

   int vertIdxs[3];
   int texIdxs[3];
   bool hasTexCoords;
   Vector3D normal;
   uint32_t smoothGroup;
};

typedef vector<Face*>::const_iterator FaceIter;

class SmoothingGroup {

public:
   ~SmoothingGroup();

   void addFace(Face* f);
   void normalize();
   Vector3D interpolateNormal(const Face* face, const double beta, const double gamma);

   map<int, Vector3D*> normals;
};

class Mesh {

public:
   Mesh();
   virtual ~Mesh();

   GLuint createDisplayList();

   void addPoint(Point3D* p) { points.push_back(p); }
   void addTextureCoord(Point2D* t) { texCoords.push_back(t); }
   void addFace(Face* f) { faces.push_back(f); }

   FaceIter facesBegin() const { return faces.begin(); }
   FaceIter facesEnd() const { return faces.end(); }
   Point3D* getPointAt(int idx) { return points[idx]; }

   void setMaterial(Material* m);
   void calculateNormals();

   void setSmoothingGroup(int group);
   map<unsigned int, SmoothingGroup*> smoothingGroups;

private:
   vector<Point3D*> points;
   vector<Point2D*> texCoords;
   vector<Face*> faces;
   Material* material;
};

#endif
