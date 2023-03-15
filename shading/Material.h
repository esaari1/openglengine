#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "Color.h"
#include "libjson/libjson.h"
#include <string>

using namespace std;

class Material {

public:
   Material();
   Material(const Material& m);

   void loadFromJSon(const JSONNode& node);
   void applyMaterial() const;
   void cleanup() const;

   Color ambient;
   Color diffuse;
   Color specular;
   Color emission;
   float shininess;
   int texID;
   bool useTexture;

private:
   void loadColor(const JSONNode& node, const string& name, Color& color);
};

#endif
