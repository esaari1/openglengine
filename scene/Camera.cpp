#include "Camera.h"

#ifdef WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif
#include <GL/gl.h>

#include <math.h>

void Camera::set(const Point3D &eye, const Point3D &look, const Vector3D &up) {
   eVec.set(eye.x, eye.y, eye.z);
   n = eye - look;
   u = up.cross(n);
   n.normalize();
   u.normalize();
   v = n.cross(u).normalize();
   setModelViewMatrix();
}

void Camera::setModelViewMatrix() {
   m.m11() = u.x;    m.m12() = u.y;    m.m13() = u.z;    m.m14() = -eVec.dot(u);
   m.m21() = v.x;    m.m22() = v.y;    m.m23() = v.z;    m.m24() = -eVec.dot(v);
   m.m31() = n.x;    m.m32() = n.y;    m.m33() = n.z;    m.m34() = -eVec.dot(n);
   m.m41() = 0.0;    m.m42() = 0.0;    m.m43() = 0.0;    m.m44() = 1.0;
}

void Camera::undoTranslate() {
   m.m14() = m.m24() = m.m34() = 0.0;
   glLoadMatrixd(m.get());
}

void Camera::loadGLMatrix() {
   glLoadMatrixd(m.get());
}

void Camera::setPosition(const Point3D& p) {
   eVec.set(p.x, p.y, p.z);
   setModelViewMatrix();
}

void Camera::slide(double delU, double delV, double delN) {
   eVec.x += delU * u.x + delV * v.x + delN * n.x;
   eVec.y += delU * u.y + delV * v.y + delN * n.y;
   eVec.z += delU * u.z + delV * v.z + delN * n.z;
   setModelViewMatrix();
}

void Camera::roll(double angle) {
   float cs = cos(M_PI / 180.0 * angle);
   float sn = sin(M_PI / 180.0 * angle);
   Vector3D t = u;
   u.set(cs * t.x + sn * v.x, cs * t.y + sn * v.y, cs * t.z + sn * v.z);
   v.set(cs * v.x - sn * t.x, cs * v.y - sn * t.y, cs * v.z - sn * t.z);
   setModelViewMatrix();
}

void Camera::pitch(double angle) {
   float cs = cos(M_PI / 180.0 * angle);
   float sn = sin(M_PI / 180.0 * angle);
   Vector3D t = n;
   n.set(cs * t.x + sn * v.x, cs * t.y + sn * v.y, cs * t.z + sn * v.z);
   v.set(cs * v.x - sn * t.x, cs * v.y - sn * t.y, cs * v.z - sn * t.z);
   setModelViewMatrix();
}

void Camera::yaw(double angle) {
   float cs = cos(M_PI / 180.0 * angle);
   float sn = sin(M_PI / 180.0 * angle);
   Vector3D t = u;
   u.set(cs * t.x + sn * n.x, cs * t.y + sn * n.y, cs * t.z + sn * n.z);
   n.set(cs * n.x - sn * t.x, cs * n.y - sn * t.y, cs * n.z - sn * t.z);
   setModelViewMatrix();
}
