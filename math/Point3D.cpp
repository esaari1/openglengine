#include "Point3D.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>

static double ERROR = 0.0001;

Point3D::Point3D(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {
}

Point3D::Point3D(const Point3D& p) {
   set(p.x, p.y, p.z);
}

void Point3D::set(double xx, double yy, double zz) {
   x = xx;
   y = yy;
   z = zz;
}

double Point3D::distanceSquared(const Point3D& p) const {
   return ( (x - p.x) * (x - p.x)
           +   (y - p.y) * (y - p.y)
           +   (z - p.z) * (z - p.z) );
}

Point3D& Point3D::operator+= (const Vector3D& v) {
   this->set(x + v.x, y + v.y, z + v.z);
   return *this;
}

Point3D& Point3D::operator*= (const double d) {
   set(x * d, y * d, z * d);
   return *this;
}

Point3D& Point3D::operator= (const Point3D& p) {
   this->set(p.x, p.y, p.z);
   return *this;
}

bool Point3D::operator== (const Point3D& p) const {
   if(fabs(x - p.x) > ERROR) {
      return false;
   }
   if(fabs(y - p.y) > ERROR) {
      return false;
   }
   if(fabs(z - p.z) > ERROR) {
      return false;
   }
   return true;
}

double Point3D::get(int axis) const {
   assert(axis >= 0 && axis <= 2);
   if(axis == 0) return x;
   if(axis == 1) return y;
   return z;
}
