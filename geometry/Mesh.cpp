#include "Mesh.h"
#include "math/Vector3D.h"
#include "math/Point3D.h"
#include "math/Point2D.h"
#include "shading/Material.h"
#include <stdio.h>

typedef vector<Vector3D*>::const_iterator VectorIter;
typedef map<unsigned int, SmoothingGroup*>::const_iterator SmoothingGroupIter;
typedef map<int, Vector3D*>::const_iterator SGNormalIter;

// Face class
Face::Face(Mesh* parent, int idx1, int idx2, int idx3) {
   vertIdxs[0] = idx1;
   vertIdxs[1] = idx2;
   vertIdxs[2] = idx3;
   texIdxs[0] = texIdxs[1] = texIdxs[2] = 0;
   hasTexCoords = false;
   smoothGroup = 0;

   Point3D* p1 = parent->getPointAt(idx1);
   Point3D* p2 = parent->getPointAt(idx2);
   Point3D* p3 = parent->getPointAt(idx3);

   Vector3D p1p2 = *p2 - *p1;
   Vector3D p1p3 = *p3 - *p1;
   normal = p1p2.cross(p1p3);
   if(normal.length() == 0.0) {
      normal.set(0, 1, 0);
   }
   else {
      normal.normalize();
   }
}

void Face::setTextureIdxs(int idx1, int idx2, int idx3) {
   texIdxs[0] = idx1;
   texIdxs[1] = idx2;
   texIdxs[2] = idx3;
   hasTexCoords = true;
}

// Smooting Group
SmoothingGroup::~SmoothingGroup() {
   for(SGNormalIter it = normals.begin(), end = normals.end(); it != end; it++) {
      delete (*it).second;
   }
   normals.clear();
}

void SmoothingGroup::addFace(Face* f) {
   for(int i = 0; i < 3; i++) {
      int vidx = f->vertIdxs[i];

      if(normals.find(vidx) == normals.end()) {
         normals[vidx] = new Vector3D();
      }
      *normals[vidx] += f->normal;
   }
}

void SmoothingGroup::normalize() {
   for(SGNormalIter sit = normals.begin(), end = normals.end(); sit != end; sit++) {
      (*sit).second->normalize();
   }
}

Vector3D SmoothingGroup::interpolateNormal(const Face* face, const double beta, const double gamma) {
   Vector3D normal(*normals[face->vertIdxs[0]] * (1.0 - beta - gamma)
                   + *normals[face->vertIdxs[1]] * beta
                   + *normals[face->vertIdxs[2]] * gamma);
   normal.normalize();
   return normal;
}

// Mesh class
Mesh::Mesh() : material(NULL) {
}

Mesh::~Mesh() {
   for(unsigned int i = 0; i < points.size(); i++) {
      delete points[i];
   }
   points.clear();

//   for(unsigned int i = 0; i < normals.size(); i++) {
//      delete normals[i];
//   }
//   normals.clear();

   delete material;
}

void Mesh::setMaterial(Material* m) {
   material = new Material(*m);
}

void Mesh::calculateNormals() {
/*
   normals.reserve(points.size());
   for(unsigned int i = 0; i < points.size(); i++) {
      normals.push_back(new Vector3D());
   }

   for(FaceIter fi = faces.begin(); fi != faces.end(); fi++) {
      int idx1 = (*fi)->vertIdxs[0];
      int idx2 = (*fi)->vertIdxs[1];
      int idx3 = (*fi)->vertIdxs[2];

      Point3D* p0 = points[idx1];
      Point3D* p1 = points[idx2];
      Point3D* p2 = points[idx3];

      Vector3D n(*p1 - *p0);
      n = n.cross(*p2 - *p1);
      n.normalize();

      *normals[idx1] += n;
      *normals[idx2] += n;
      *normals[idx3] += n;
   }

   for(VectorIter it = normals.begin(); it != normals.end(); it++) {
      (*it)->normalize();
   }
*/
   if(!smoothingGroups.empty()) {
      for(SmoothingGroupIter it = smoothingGroups.begin(), end = smoothingGroups.end(); it != end; ++it) {
         (*it).second->normalize();
      }
   }
 }

GLuint Mesh::createDisplayList() {
   GLuint drawId = glGenLists(1);
   glNewList(drawId, GL_COMPILE);
   glPushAttrib(GL_LIGHTING_BIT);

   // Begin draw routine
   material->applyMaterial();

   glBegin(GL_TRIANGLES);

   for(FaceIter fi = faces.begin(); fi != faces.end(); fi++) {
      for(unsigned int i = 0; i < 3; i++) {
         int idx = (*fi)->vertIdxs[i];
         if((*fi)->smoothGroup == 0) {
            glNormal3f((*fi)->normal.x, (*fi)->normal.y, (*fi)->normal.z);
         }
         else {
            SmoothingGroup* sg = smoothingGroups[(*fi)->smoothGroup];
            Vector3D* n = sg->normals[idx];
            glNormal3f(n->x, n->y, n->z);
         }

         if((*fi)->hasTextureCoords()) {
            Point2D* t = texCoords[(*fi)->texIdxs[i]];
            glTexCoord2f(t->x, t->y);
         }

         const Point3D* p = points[idx];
         glVertex3f(p->x, p->y, p->z);
      }
   }

   material->cleanup();
   glEnd();

   // End draw routine
   glPopAttrib();
   glEndList();

   return drawId;
}
