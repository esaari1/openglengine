#ifndef M3DSMESH_H_
#define M3DSMESH_H_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "shading/Color.h"

using namespace std;

class Mesh;
class Material;

class M3DSParser {

public:
   M3DSParser(const string& p);
   virtual ~M3DSParser();

   vector<GLuint> load(const string& filename);

private:
   void processTopLevelChunk(int nBytes);
   void processSceneChunk(int nBytes);
   void processModelChunk(int nBytes);
   void processMaterialChunk(int nBytes);
   void processMeshChunk(int nBytes);
   void processFaceArrayChunk(int nBytes, Mesh* mesh);
   Color processColorChunk(int nBytes);
   float processPercentageChunk(int nBytes);
   GLuint processTexmapChunk(int nBytes);

   void readColor(Color& color);
   void readFloatColor(Color& color);
//   Matrix readMatrix();

   void readPointArray(Mesh* mesh);
   void readTextureCoordArray(Mesh* mesh);
   void readFaceArray(Mesh* mesh, int contentSize);

   void skipBytes(int count);

   string path;
   ifstream in;
   vector<GLuint> model;
   map<string, Material*> materials;
};

#endif /*M3DSMESH_H_*/
