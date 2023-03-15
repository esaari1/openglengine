#ifndef _COLOR_H
#define _COLOR_H

class Color {

public:
   Color(float r = 1, float g = 1, float b = 1, float a = 1) { c[0] = r; c[1] = g; c[2] = b; c[3] = a; }
   operator const float* () const { return c; }
   Color& operator= (const Color& o);
   Color& operator*= (float p);
   void set(float r = 1, float g = 1, float b = 1, float a = 1) { c[0] = r; c[1] = g; c[2] = b; c[3] = a; }

   float c[4];
};

inline Color& Color::operator= (const Color& o) {
   c[0] = o.c[0];
   c[1] = o.c[1];
   c[2] = o.c[2];
   c[3] = o.c[3];
   return *this;
}

inline Color& Color::operator*= (float p) {
   c[0] *= p;
   c[1] *= p;
   c[2] *= p;
   return *this;
}

#endif
