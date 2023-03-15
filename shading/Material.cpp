#include "Material.h"
#ifdef WIN32
   #define GLEW_STATIC 1
   #include <GL/glew.h>
#endif
#include <GL/gl.h>

#include "utility/Texture.h"

Material::Material() :
   ambient(0, 0, 0, 1),
   diffuse(1, 1, 1, 1),
   specular(0, 0, 0, 1),
   emission(0, 0, 0, 1),
   shininess(100.0f),
   texID(-1),
   useTexture(false)
{
}

Material::Material(const Material& m) {
   ambient = m.ambient;
   diffuse = m.diffuse;
   specular = m.specular;
   emission = m.emission;
   shininess = m.shininess;
   texID = m.texID;
   useTexture = m.useTexture;
}

void Material::loadFromJSon(const JSONNode& node) {
   JSONNode::const_iterator it = node.find("texture");
   if(it != node.end()) {
      GLint mode = GL_MODULATE;
      if(node.find("texEnv") != node.end()) {
         string env = node.find("texEnv")->as_string();
         if(env == "replace") mode = GL_REPLACE;
      }
      texID = loadTexture(it->as_string(), mode);
      useTexture = true;
   }

   it = node.find("shininess");
   if(it != node.end()) {
      shininess = it->as_float();
   }

   loadColor(node, "ambient", ambient);
   loadColor(node, "diffuse", diffuse);
   loadColor(node, "specular", specular);
   loadColor(node, "emission", emission);
}

void Material::loadColor(const JSONNode& node, const string& name, Color& color) {
   JSONNode::const_iterator it = node.find(name);
   if(it != node.end()) {
      color.set(it->at(0).as_float(), it->at(1).as_float(), it->at(2).as_float(), it->at(3).as_float());
   }
}

void Material::applyMaterial() const {
   glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
   glMaterialfv(GL_FRONT, GL_EMISSION, emission);
   glMaterialf(GL_FRONT, GL_SHININESS, shininess);

   if(useTexture && texID != -1) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, texID);
   }
}

void Material::cleanup() const {
   if(useTexture && texID != -1) {
      glDisable(GL_TEXTURE_2D);
   }
}
