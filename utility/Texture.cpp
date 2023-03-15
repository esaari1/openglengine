#include "Texture.h"
#include <SDL/SDL_image.h>

GLuint loadTexture(string fname, GLint envMode) {
   SDL_Surface* surf = IMG_Load(fname.c_str());

   int internalFormat = 0;
   int format = 0;
   if (surf->format->BytesPerPixel == 1) {
      // convert to an RGBA surface
      SDL_Surface* newSurf = SDL_DisplayFormat(surf);
      SDL_FreeSurface(surf);
      surf = newSurf;
   }

   if (surf->format->BytesPerPixel == 3) { // RGB 24bit
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      format = GL_RGB;
#else
      format = GL_BGR;
#endif
      internalFormat = GL_RGB;
   } else if (surf->format->BytesPerPixel == 4) { // RGBA 32bit
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      format = GL_RGBA;
#else
      format = GL_BGRA;
#endif
      internalFormat = GL_RGBA;
   } else {
      printf("Unknown surface format %d\n", surf->format->BytesPerPixel);
      SDL_FreeSurface(surf);
      return 0;
   }

   GLuint glid;
   glGenTextures(1, &glid);
   glBindTexture(GL_TEXTURE_2D, glid);
   glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, surf->w, surf->h, 0, format, GL_UNSIGNED_BYTE, surf->pixels);

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, envMode);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   SDL_FreeSurface(surf);
   return glid;
}