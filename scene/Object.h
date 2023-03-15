#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <string>
#include <vector>
#include "libjson/libjson.h"
#include "shading/Material.h"
#include "math/Point3D.h"
#include "math/Vector3D.h"

using namespace std;

class Object {

public:
   Object(string n);
   static Object* load(const JSONNode& node);

   void setPosition(const Point3D& pos);
   void setRotation(const Vector3D& rot);
   void draw() const;
   string getName() const { return name; }

protected:
   virtual void setup(const JSONNode& node) = 0;
   virtual void drawObject() const = 0;

   string name;
   string shader;
   float tx, ty, tz;
   float sx, sy, sz;
   float rx, ry, rz;
   bool scale;
};

class MeshObject : public Object {

public:
   MeshObject(string n) : Object(n) {}

protected:
   virtual void setup(const JSONNode& node);
   virtual void drawObject() const;
};

class SphereObject : public Object {

public:
   SphereObject(string n) : Object(n) {}

protected:
   virtual void setup(const JSONNode& node);
   virtual void drawObject() const;

private:
   Material material;
};

class SquareObject : public Object {

public:
   SquareObject(string n) : Object(n) {}

protected:
   virtual void setup(const JSONNode& node);
   virtual void drawObject() const;

private:
   Material material;
};

#endif