#ifndef _LIGHTWAVE_PARSER_H_
#define _LIGHTWAVE_PARSER_H_

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

class Material;
class Mesh;

class LightWaveParser {

public:
   LightWaveParser(const string p);

   vector<GLuint> load(const string& filename);

private:
   int parseTags();
   int parsePTag();
   int parsePoints(Mesh* mesh);
   int parsePolygons(Mesh* mesh);
   int parseSurface();
   int skipChunk();

   string path;
   ifstream in;
   vector<string> tags;
   vector<GLuint> model;
   map<string, Material*> materialMap;
};

#endif
