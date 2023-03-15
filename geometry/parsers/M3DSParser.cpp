#include "M3DSParser.h"
#include <iomanip>
#include "utility/3dschunk.h"
#include "utility/ChunkParser.h"
#include "math/Point2D.h"
#include "math/Point3D.h"
#include "geometry/Mesh.h"
#include "shading/Material.h"
#include "utility/Texture.h"

//#define PRINT_UNPROCESSED

typedef map<string, Material*>::const_iterator MatIter;

M3DSParser::M3DSParser(const string& p) : path(p), in(), model()
{
}

M3DSParser::~M3DSParser() {
   for(MatIter it = materials.begin(); it != materials.end(); ++it) {
      delete it->second;
   }
   materials.clear();
}

vector<GLuint> M3DSParser::load(const string& filename) {
   string fname = path + filename;

   in.open(fname.c_str(), ios::in | ios::binary);
   if (!in.good()) {
      fprintf(stderr, "Read3DSFile: Error opening %s\n", filename.c_str());
   }
   else {
      uint16 chunkType = readUshortLE(in);
      if (chunkType != M3DCHUNK_MAGIC) {
         fprintf(stderr, "Read3DSFile: Wrong magic number in header\n");
         return model;
      }

      int chunkSize = readIntLE(in);
      if (in.bad()) {
         fprintf(stderr, "Read3DSFile: Error reading 3DS file.\n");
         return model;
      }

      int contentSize = chunkSize - 6;
      processTopLevelChunk(contentSize);

      in.close();
   }

   return model;
}

void M3DSParser::processTopLevelChunk(int nBytes) {
   int bytesRead = 0;

   while (bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_MESHDATA) {
         processSceneChunk(contentSize);
      }
      else {
         #ifdef PRINT_UNPROCESSED
            printf("processTopLevelChunk %X %d\n", chunkType, chunkSize);
         #endif
         skipBytes(contentSize);
      }
   }

   if (bytesRead != nBytes) {
      cerr << "In processTopLevelChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }
}

void M3DSParser::processSceneChunk(int nBytes) {
   int bytesRead = 0;

   while (bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_NAMED_OBJECT) {
         string name = readString(in);
         processModelChunk(contentSize - (int)(name.length() + 1));
      }
      else if (chunkType == M3DCHUNK_MATERIAL_ENTRY) {
         processMaterialChunk(contentSize);
      }
      else {
         #ifdef PRINT_UNPROCESSED
            printf("processSceneChunk %X %d\n", chunkType, chunkSize);
         #endif
         skipBytes(contentSize);
      }
   }

   if (bytesRead != nBytes) {
      cerr << "In processSceneChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }
}

void M3DSParser::processModelChunk(int nBytes) {
   int bytesRead = 0;

   while(bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_TRIANGLE_MESH) {
         processMeshChunk(contentSize);
      }
      else {
         skipBytes(contentSize);
      }
   }

   if(bytesRead != nBytes) {
      cerr << "In processModelChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }
}

void M3DSParser::processMaterialChunk(int nBytes) {
   int bytesRead = 0;
   Material* material = new Material();
   float shinePercent = 1.f;

   while(bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_MATERIAL_NAME) {
         string name = readString(in);
         materials[name] = material;
      }
      else if (chunkType == M3DCHUNK_MATERIAL_AMBIENT) {
         material->ambient = processColorChunk(contentSize);
      }
      else if (chunkType == M3DCHUNK_MATERIAL_DIFFUSE) {
         material->diffuse = processColorChunk(contentSize);
      }
      else if (chunkType == M3DCHUNK_MATERIAL_SPECULAR) {
         material->specular = processColorChunk(contentSize);
      }
      else if (chunkType == M3DCHUNK_MATERIAL_SHININESS) {
         material->shininess = processPercentageChunk(contentSize) * 100.f;
      }
      else if (chunkType == M3DCHUNK_MATERIAL_SHIN2PCT) {
         shinePercent = processPercentageChunk(contentSize);
      }
//      else if (chunkType == M3DCHUNK_MATERIAL_TRANSPARENCY) {
//         material.opacity = processPercentageChunk(contentSize);
//      }
      else if (chunkType == M3DCHUNK_MATERIAL_TEXMAP) {
         material->texID = processTexmapChunk(contentSize);
      }
      else {
         #ifdef PRINT_UNPROCESSED
            printf("processMaterialChunk %X %d\n", chunkType, chunkSize);
         #endif
         skipBytes(contentSize);
      }
   }

   material->specular *= shinePercent;

   if(bytesRead != nBytes) {
      cerr << "In processMaterialChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }
}

void M3DSParser::processMeshChunk(int nBytes) {
   int bytesRead = 0;
   Mesh* mesh = new Mesh();

   while(bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_POINT_ARRAY) {
         readPointArray(mesh);
      }
      else if (chunkType == M3DCHUNK_MESH_TEXTURE_COORDS) {
         readTextureCoordArray(mesh);
      }
      else if (chunkType == M3DCHUNK_FACE_ARRAY) {
         readFaceArray(mesh, contentSize);
      }
      else {
         #ifdef PRINT_UNPROCESSED
            printf("processMeshChunk %X %d\n", chunkType, chunkSize);
         #endif
         skipBytes(contentSize);
      }
   }

   if(bytesRead != nBytes) {
      cerr << "In processTriMeshChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }

   mesh->calculateNormals();
   model.push_back(mesh->createDisplayList());
   delete mesh;
}

void M3DSParser::processFaceArrayChunk(int nBytes, Mesh* mesh) {
   int bytesRead = 0;

   while(bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_MESH_MATERIAL_GROUP) {
         // For now, we just assume that there is only one material group
         // per triangle mesh, and that the material applies to all faces in
         // the mesh.
         string materialName = readString(in);
         uint16 nFaces = readUshortLE(in);

         for (uint16 i = 0; i < nFaces; i++) {
            readUshortLE(in);
         }

         mesh->setMaterial(materials[materialName]);
      }
      else if(chunkType == M3DCHUNK_MESH_SMOOTH_GROUP) {
         for(FaceIter it = mesh->facesBegin(), end = mesh->facesEnd(); it != end; it++) {
            unsigned int group = readUIntLE(in);
            unsigned mask = 1;
            for(int i = 0; i < 32; i++) {
               if(mask & group) {
                  if(mesh->smoothingGroups.find(group) == mesh->smoothingGroups.end()) {
                     mesh->smoothingGroups[group] = new SmoothingGroup();
                  }
                  (*it)->smoothGroup = group;
                  mesh->smoothingGroups[group]->addFace(*it);
               }
               mask = mask << 1;
            }
         }
      }
      else {
         #ifdef PRINT_UNPROCESSED
            printf("processFaceArrayChunk %X %d\n", chunkType, chunkSize);
         #endif
         skipBytes(contentSize);
      }
   }

   if(bytesRead != nBytes) {
      cerr << "In processFaceArrayChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }
}

Color M3DSParser::processColorChunk(int nBytes) {
   int bytesRead = 0;
   Color color;

   while (bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_COLOR_24) {
         readColor(color);
      }
      else if (chunkType == (M3DCHUNK_COLOR_FLOAT)) {
         readFloatColor(color);
      }
      else {
#ifdef PRINT_UNPROCESSED
         printf("processColorChunk %X %d\n", chunkType, chunkSize);
#endif
         skipBytes(contentSize);
      }
   }

   if(bytesRead != nBytes) {
      cerr << "In processColorChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }

   return color;
}

float M3DSParser::processPercentageChunk(int nBytes) {
   int bytesRead = 0;
   float percent;

   while (bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_INT_PERCENTAGE) {
         int p = readShortLE(in);
         percent = p / 100.0f;
      }
      else if (chunkType == M3DCHUNK_FLOAT_PERCENTAGE) {
         percent = readFloatLE(in);
      }
      else {
         #ifdef PRINT_UNPROCESSED
            printf("processPercentageChunk %X %d\n", chunkType, chunkSize);
         #endif
         skipBytes(contentSize);
      }
   }

   if(bytesRead != nBytes) {
      cerr << "In processPercentageChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }

   return percent;
}

GLuint M3DSParser::processTexmapChunk(int nBytes) {
   int bytesRead = 0;
   GLuint texID = 0;

   while (bytesRead < nBytes) {
      uint16 chunkType = readUshortLE(in);
      int chunkSize = readIntLE(in);
      int contentSize = chunkSize - 6;
      bytesRead += chunkSize;

      if (chunkType == M3DCHUNK_MATERIAL_MAPNAME) {
         string texmap = path + readString(in);
         texID = loadTexture(texmap);
      }
      else {
         #ifdef PRINT_UNPROCESSED
            printf("processTexmapChunk %X %d\n", chunkType, chunkSize);
         #endif
         skipBytes(contentSize);
      }
   }

   if(bytesRead != nBytes) {
      cerr << "In processTexmapChunk expected " << nBytes << " bytes but read " << bytesRead << '\n';
   }

   return texID;
}

void M3DSParser::readColor(Color& color) {
   uint8 r = (uint8) readChar(in);
   uint8 g = (uint8) readChar(in);
   uint8 b = (uint8) readChar(in);

   color.c[0] = (float) r / 255.0f;
   color.c[1] = (float) g / 255.0f;
   color.c[2] = (float) b / 255.0f;
}


void M3DSParser::readFloatColor(Color& color) {
   color.c[0] = readFloatLE(in) / 255.0f;
   color.c[1] = readFloatLE(in) / 255.0f;
   color.c[2] = readFloatLE(in) / 255.0f;
}

void M3DSParser::readPointArray(Mesh* mesh) {
   uint16 nPoints = readUshortLE(in);

   for (int i = 0; i < (int) nPoints; i++) {
      float x = readFloatLE(in);
      float y = readFloatLE(in);
      float z = readFloatLE(in);
      mesh->addPoint(new Point3D(x, y, z));
   }
}

void M3DSParser::readTextureCoordArray(Mesh* mesh) {
   uint16 nPoints = readUshortLE(in);

   for (int i = 0; i < (int) nPoints; i++) {
      float s = readFloatLE(in);
      float t = readFloatLE(in);
      mesh->addTextureCoord(new Point2D(s, t));
   }
}

void M3DSParser::readFaceArray(Mesh* mesh, int contentSize) {
   uint16 nFaces = readUshortLE(in);

   for (int i = 0; i < (int) nFaces; i++) {
      uint16 v0 = readUshortLE(in);
      uint16 v1 = readUshortLE(in);
      uint16 v2 = readUshortLE(in);
      /* uint16 flags = */readUshortLE(in);
      mesh->addFace(new Face(mesh, v0, v1, v2));
   }

   int bytesLeft = contentSize - (8 * nFaces + 2);
   if (bytesLeft > 0) {
      processFaceArrayChunk(bytesLeft, mesh);
   }
}

void M3DSParser::skipBytes(int count) {
   char c;
   while (count-- > 0) {
      in.get(c);
   }
}
