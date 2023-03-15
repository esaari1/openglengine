#ifndef _POINT2D_H_
#define _POINT2D_H_

class Point2D {

public:
   Point2D(float _x = 0, float _y = 0) : x(_x), y(_y) {}
   
   Point2D operator*(const float a) const;
   Point2D& operator*=(const float a);
   
   float x, y;
};

inline Point2D Point2D::operator*(const float a) const {
   return Point2D(x * a, y * a);
}

inline Point2D& Point2D::operator*=(const float a) {
   x *= a;
   y *= a;
   return *this;
}

#endif
