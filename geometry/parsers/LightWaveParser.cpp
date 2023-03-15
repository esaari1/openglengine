#include "LightWaveParser.h"
#include "utility/ChunkParser.h"
#include "shading/Material.h"
#include "math/Point3D.h"
#include "geometry/Mesh.h"

LightWaveParser::LightWaveParser(const string p) : path(p), in() {
}

vector<GLuint> LightWaveParser::load(const string& filename) {
   string fname = path + filename;

   in.open(fname.c_str(), ios::in | ios::binary);
   if (!in.good()) {
      fprintf(stderr, "Lightwave Parser: Error opening %s\n", fname.c_str());
   }
   else {
      string chunkID = readChunkID(in, 4); // FORM
      int size = readIntBE(in);
      int count = 0;

      readChunkID(in, 4); // LWO2
      count += 4;

      Mesh* mesh = new Mesh();

      while(count < size) {
         chunkID = readChunkID(in, 4);
         count += 4;

         printf("ID = %s\n", chunkID.c_str());

         if(chunkID == "TAGS") {
            count += parseTags();
         }
         else if(chunkID == "PNTS") {
            count += parsePoints(mesh);
         }
         else if(chunkID == "POLS") {
            count += parsePolygons(mesh);
         }
         else if(chunkID == "PTAG") {
            parsePTag();
         }
         else if(chunkID == "SURF") {
            parseSurface();
         }
         else {
            count += skipChunk();
         }
      }

      in.close();
   }

   return model;
}

int LightWaveParser::parseTags() {
   int size = readIntBE(in);
   int count = 4;

   int idx = 0;

   while(count < size) {
      string tagName = readString(in);
//      printf("%d = %s, %d\n", idx++, tagName.c_str());
      tags.push_back(tagName);
      count += tagName.size() + 1;

      if(tagName.length() % 2 == 0) {
         readChar(in);
         count++;
      }
   }

   return count;
}

int LightWaveParser::parsePTag() {
   int size = readIntBE(in);
   int count = 4;
   string subtag = readChunkID(in, 4);

   if(subtag == "SURF") {
      int vsize;
      while(count < size) {
         unsigned int fidx = readVariableUIntBE(in, vsize);
         count += vsize;
         uint16 tidx = readUshortBE(in);
         printf("face %d uses %d\n", fidx, tidx);
//         faces[fidx]->materialName = tags[tidx];
         count += 2;
      }
   }
   else {
      char c;
      while(count < size) {
         c = readChar(in);
         count++;
      }
   }

   return size + 4;
}

int LightWaveParser::parsePoints(Mesh* mesh) {
   int size = readIntBE(in);
   int count = 4;

   while(count < size) {
      float x = readFloatBE(in);
      float y = readFloatBE(in);
      float z = readFloatBE(in);
      mesh->addPoint(new Point3D(x, y, z));
      count += 12;
   }

   return count;
}

int LightWaveParser::parsePolygons(Mesh* mesh) {
   int size = readIntBE(in);
   int count = 4;

   string type = readChunkID(in, 4);
   count += 4;
   int vsize;

   while(count < size) {
      short pointCnt = readShortBE(in);
      count += 2;

      unsigned int i1 = readVariableUIntBE(in, vsize);
      count += vsize;

      unsigned int i2 = readVariableUIntBE(in, vsize);
      count += vsize;

      for(int p = 2; p < pointCnt; p++) {
         unsigned int i3 = readVariableUIntBE(in, vsize);
         count += vsize;
         mesh->addFace(new Face(mesh, i1, i2, i3));
         i2 = i3;
      }
   }

   return count;
}

int LightWaveParser::parseSurface() {
   int size = readIntBE(in);
   int count = 4;

   string sname = readString(in);
   count += sname.size() + 1;
   if(sname.size() % 2 == 0) {
      count++;
      readChar(in);
   }

   string pname = readString(in);
   count += pname.size() + 1;
   if(pname.size() % 2 == 0) {
      count++;
      readChar(in);
   }

   Color color(1, 1, 1);
   float diffuse = 1.f;

   while(count < size) {
      string subName = readChunkID(in, 4);
      short subSize = readShortBE(in);
      count += 6;

      if(subName == "COLR") {
         float r = readFloatBE(in);
         float b = readFloatBE(in);
         float g = readFloatBE(in);
         readShortBE(in);
         color.set(r, g, b);
      }
      else if(subName == "DIFF") {
         diffuse = readFloatBE(in);
         readShortBE(in);
      }
      else {
         printf("subName = %s\n", subName.c_str());
         for(int i = 0; i < subSize; i++) {
            readChar(in);
         }
      }
      count += subSize;
   }

   color *= diffuse;
//   materialMap[sname]->setColor(color.red, color.green, color.blue);

   return count;
}

int LightWaveParser::skipChunk() {
   int count = 0;
   int size = readIntBE(in);
   char c;

   while(count++ < size) {
      c = readChar(in);
   }

   return size + 4;
}
