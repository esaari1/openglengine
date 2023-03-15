#include "Object.h"
#include "geometry/MeshManager.h"
#include "utility/Texture.h"
#include "shading/ShaderManager.h"

Object::Object(string n) : name(n), shader("none"), tx(0), ty(0), tz(0), sx(1), sy(1), sz(1), rx(0), ry(0), rz(0), scale(false) {
}

Object* Object::load(const JSONNode& node) {
   string type = node.find("type")->as_string();
   string name = node.find("name")->as_string();

   Object* obj;

   if(type == "mesh") {
      obj = new MeshObject(name);
   }
   else if(type == "sphere") {
      obj = new SphereObject(name);
   }
   else if(type == "square") {
      obj = new SquareObject(name);
   }

   JSONNode::const_iterator it = node.find("translate");
   if(it != node.end()) {
      JSONNode array = it->as_array();
      obj->tx = array.at(0).as_float();
      obj->ty = array.at(1).as_float();
      obj->tz = array.at(2).as_float();
   }

   it = node.find("scale");
   if(it != node.end()) {
      obj->scale = true;
      JSONNode array = it->as_array();
      obj->sx = array.at(0).as_float();
      obj->sy = array.at(1).as_float();
      obj->sz = array.at(2).as_float();
   }

   it = node.find("rotate");
   if(it != node.end()) {
      JSONNode array = it->as_array();
      obj->rx = array.at(0).as_float();
      obj->ry = array.at(1).as_float();
      obj->rz = array.at(2).as_float();
   }

   it = node.find("shader");
   if(it != node.end()) {
      obj->shader = it->as_string();
   }

   obj->setup(node);

   return obj;
}

void Object::setPosition(const Point3D& pos) {
   tx = pos.x;
   ty = pos.y;
   tz = pos.z;
}

void Object::setRotation(const Vector3D& rot) {
   rx = rot.x;
   ry = rot.y;
   rz = rot.z;
}

void Object::draw() const {
   ShaderManager::instance().setActiveShader(shader);

   glPushMatrix();
   glTranslatef(tx, ty, tz);
   glRotatef(rx, 1, 0, 0);
   glRotatef(ry, 0, 1, 0);
   glRotatef(rz, 0, 0, 1);

   if(scale) {
      glEnable(GL_RESCALE_NORMAL);
      glScalef(sx, sy, sz);
   }

   drawObject();

   if(scale) {
      glDisable(GL_RESCALE_NORMAL);
   }
   glPopMatrix();

   ShaderManager::instance().setActiveShader("none");
}

void MeshObject::setup(const JSONNode& node) {
}

void MeshObject::drawObject() const {
   vector<GLuint> lists = MeshManager::instance().getMesh(name);
   for(int i = 0; i < lists.size(); i++) {
      glCallList(lists[i]);
   }
}

void SphereObject::setup(const JSONNode& node) {
   material.loadFromJSon(node);
}

void SphereObject::drawObject() const {
   material.applyMaterial();

   GLuint list = MeshManager::instance().getMesh("sphere15")[0];
   glCallList(list);

   material.cleanup();
}

void SquareObject::setup(const JSONNode& node) {
   material.loadFromJSon(node);
}

void SquareObject::drawObject() const {
   material.applyMaterial();

   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);

   glTexCoord2d(0, 1); glVertex3f(-1, -1, 0);
   glTexCoord2d(1, 1); glVertex3f(1, -1, 0);
   glTexCoord2d(1, 0); glVertex3f(1, 1, 0);
   glTexCoord2d(0, 0); glVertex3f(-1, 1, 0);

   glEnd();

   material.cleanup();
}

