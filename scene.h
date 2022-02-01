/* File: scene.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;            */

#ifndef SCENE_H
#define SCENE_H

#define _CRT_SECURE_NO_DEPRECATE

#include <assert.h>

#include "math.h"

#define MAX_PARSER_TOKEN_LENGTH 100
#define SIZE 0x20

class Camera;
class Material;
class DiffuseMaterial;
class ReflectiveMaterial;
class GlassMaterial;
class NoiseMaterial;
class MarbleMaterial;
class WoodMaterial;
class Checkerboard;
class Object;
class Solid;
class Group;
class CSGPair;
class Sphere;
class MotionSphere;
class Plane;
class Cube;
class Triangle;
class Cone;
class XYRectangle;
class XZRectangle;
class YZRectangle;
class Transform;

class Scene
{
public:
   Scene(const char* szFileName);
   ~Scene();

   Camera*   GetCamera()           const {   return camera;       }
   color3f   GetBackground  ()     const {   return background;   }

   size_t    GetNumMaterials()     const {   return num_materials;      }
   Material* GetMaterial(size_t i) const {   assert(i < num_materials);   return material[i];       }
   Group*    GetGroup()            const {   return group;              }

   bool      UseSamples()          const {   return distribution;       }

private:
   void ParseFile();
   void ParseOrthographicCamera();
   void ParsePerspectiveCamera();
   void ParseBackground();
   void ParseMaterials();

   DiffuseMaterial*    ParseDiffuse();
   ReflectiveMaterial* ParseReflective();
   GlassMaterial*      ParseGlass();
   NoiseMaterial*      ParseNoise(size_t count);
   MarbleMaterial*     ParseMarble(size_t count);
   WoodMaterial*       ParseWood(size_t count);
   Checkerboard*       ParseCheckerboard(size_t count);
   Object*       ParseObject(char token[MAX_PARSER_TOKEN_LENGTH]);
   Group*        ParseGroup();
   CSGPair*      ParseCSGPair();
   Sphere*       ParseSphere();
   MotionSphere* ParseMotionSphere();
   Plane*        ParsePlane();
   Triangle*     ParseTriangle();
   Cone*         ParseCone();
   XYRectangle*  ParseXYRectangle();
   XZRectangle*  ParseXZRectangle();
   YZRectangle*  ParseYZRectangle();
   Group*        ParseTriangleMesh();
   Cube*         ParseCube();
   Transform*    ParseTransform();

   bool GetToken(char token[MAX_PARSER_TOKEN_LENGTH]);

   vector3f ReadVector3f();
   vector2f ReadVector2f();
   float    ReadFloat();
   int      ReadInt();

   FILE* file;

   Camera* camera;

   color3f background;

   size_t num_materials;
   Material* current_material;
   Material* material[SIZE];

   Group* group;

   bool distribution;
};

#endif
