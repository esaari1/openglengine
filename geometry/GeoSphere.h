#ifndef _GEOSPHERE_H_
#define _GEOSPHERE_H_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include <map>
#include <vector>
#include "math/Vector3D.h"

using namespace std;

class Point2D;
class Material;

class GeoFace {

public:
   GeoFace(int idx1, int idx2, int idx3);
   ~GeoFace();

   int vertIdxs[3];
   vector<Point2D*> texCoords;
};

class Edge {

public:
   Edge(int p1, int p2);
   Edge(const Edge& e);
   bool operator==(const Edge& e) const;
   bool operator<(const Edge& e) const;
   int pidx1, pidx2;
};

class GeoSphere {

public:
   GeoSphere(int _divs);
   ~GeoSphere();
   GLuint createDisplayList();

private:
   void addPoint(Vector3D* norm, bool addTangents = true);
   void subdivide(int pidx1, int idxp2, int pidx3);
   vector<int>* getEdgePoints(const Edge& e);
   void calculateTexCoords(GeoFace* f);

   int divs;
   map<Edge, vector<int>* > edgeMap;
   vector<GeoFace*> faces;
   vector<Point3D*> points;
   vector<Vector3D*> normals;
   vector<Vector3D*> tangents;
   vector<Vector3D*> binormals;
};

#endif
