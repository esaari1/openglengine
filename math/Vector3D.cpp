#include "Vector3D.h"
#include "Point3D.h"
#include <math.h>
#include <assert.h>

static double ERROR = 0.0001;

Vector3D::Vector3D(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {
}

Vector3D::Vector3D(const Vector3D &v) {
   set(v.x, v.y, v.z);
}

void Vector3D::set(double xx, double yy, double zz) {
   x = xx;
   y = yy;
   z = zz;
}

Vector3D& Vector3D::operator+= (const Vector3D& p) {
   this->set(x + p.x, y + p.y, z + p.z);
   return *this;
}

Vector3D& Vector3D::operator*= (const double d) {
   this->set(x * d, y * d, z * d);
   return *this;
}

Vector3D& Vector3D::operator= (const Vector3D& p) {
   x = p.x;
   y = p.y;
   z = p.z;
   return *this;
}

bool Vector3D::operator== (const Vector3D& v) {
   if(fabs(x - v.x) > ERROR) {
      return false;
   }
   if(fabs(y - v.y) > ERROR) {
      return false;
   }
   if(fabs(z - v.z) > ERROR) {
      return false;
   }
   return true;
}

Vector3D& Vector3D::normalize() {
   double invLength = 1.0 / sqrt(x*x + y*y + z*z);
   x *= invLength;
   y *= invLength;
   z *= invLength;
   return *this;
}

Vector3D& Vector3D::selfCross(const Vector3D& v) {
  double nx = (y * v.z) - (z * v.y);
  double ny = (z * v.x) - (x * v.z);
  double nz = (x * v.y) - (y * v.x);

  this->set(nx, ny, nz);
  return *this;
}

double Vector3D::get(int axis) const {
   assert(axis >= 0 && axis <= 2);
   if(axis == 0) return x;
   if(axis == 1) return y;
   return z;
}
