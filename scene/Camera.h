#ifndef _CAMERA_H
#define _CAMERA_H

#include "math/Vector3D.h"
#include "math/Point3D.h"
#include "math/Matrix.h"

class Camera {

public:
   Camera() { }

   void reset() { m.setIdentity(); }
   void set(const Point3D &eye, const Point3D &look, const Vector3D &up);
   void loadGLMatrix();
   void setPosition(const Point3D& p);
   void slide(double delU, double delV, double delN);
   void roll(double angle);
   void pitch(double angle);
   void yaw(double angle);
   void undoTranslate();
   Vector3D getLocation() const { return eVec; }

   Matrix m;

private:
   Vector3D u, v, n, eVec;

   void setModelViewMatrix();
};

#endif
