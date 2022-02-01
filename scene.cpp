/* File: scene.cpp; Mode: C++; Tab-width: 3; Author: MIT 6.837;               */

// The following logic shall parse scene files without any error handling.
// In hindsight it was a mistake to use asserts to validate tokens.

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "scene.h"
#include "camera.h"
#include "material.h"
#include "object.h"

#define DegreesToRadians(x) ((PI * x) / 180.0f)

Scene::Scene(const char* szFileName)
{
   group = NULL;
   camera = NULL;
   background = color3f(1.0f, 1.0f, 1.0f);
 
   current_material = NULL;
   num_materials = 0;

   for (size_t i = 0; i < SIZE; ++i)
   {
      material[i] = NULL;
   }

   distribution = false;

   if (szFileName != NULL)
   {
      file = fopen(szFileName, "r");

      if (file != NULL)
      {
         ParseFile();
         fclose(file);
      }
   }

   file = NULL;
}

Scene::~Scene()
{
   delete group;
   delete camera;

   for (size_t i = 0; i < num_materials; ++i)
   {
      delete material[i];
   }
}

void Scene::ParseFile()
{
/* At the top level, the scene can have a camera, 
   background color, materials, and a group of objects. */
   
   char token[MAX_PARSER_TOKEN_LENGTH];    
  
   while (GetToken(token) != false)
   { 
      if (strcmp(token, "OrthographicCamera") == 0)
      {
         ParseOrthographicCamera();
      }
      else if (strcmp(token, "PerspectiveCamera") == 0)
      {
         ParsePerspectiveCamera();
      }
      else if (strcmp(token, "Background") == 0)
      {
         ParseBackground();
      }
      else if (strcmp(token, "Materials") == 0)
      {
         ParseMaterials();
      }
      else if (strcmp(token, "Group") == 0)
      {
         group = ParseGroup();
      }
   }

   return;
}

void Scene::ParseOrthographicCamera()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   GetToken(token); assert(strcmp(token, "{") == 0);
   point3f center;
   vector3f direction, up;
   float size = 0.0f;

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "center") == 0)
      {
         center = ReadVector3f();
      }
      else if (strcmp(token, "direction") == 0)
      {
         direction = ReadVector3f();
      }
      else if (strcmp(token, "lookat") == 0)
      {
         point3f at = ReadVector3f();

         direction = Camera::LookAt(center, at);
      }
      else if (strcmp(token, "up") == 0)
      {
         up = ReadVector3f();
      }
      else if (strcmp(token, "size") == 0)
      {
         size = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   camera = new OrthographicCamera(center, direction, up, size);
}

void Scene::ParsePerspectiveCamera()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point3f center;
   vector3f direction, up;
   float angle_radians = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "center") == 0)
      {
         center = ReadVector3f();
      }
      else if (strcmp(token, "direction") == 0)
      {
         direction = ReadVector3f();
      }
      else if (strcmp(token, "lookat") == 0)
      {
         point3f at = ReadVector3f();

         direction = Camera::LookAt(center, at);
      }
      else if (strcmp(token, "up") == 0)
      {
         up = ReadVector3f();
      }
      else if (strcmp(token, "angle") == 0)
      {
         float angle_degrees = ReadFloat();
         angle_radians = DegreesToRadians(angle_degrees);
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   camera = new PerspectiveCamera(center, direction, up, angle_radians);

   return;
}

void Scene::ParseBackground()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   GetToken(token); assert(strcmp(token, "{") == 0);  

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "color") == 0)
      {
         background = ReadVector3f();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   return;
}

void Scene::ParseMaterials()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   GetToken(token); assert(strcmp(token, "{") == 0);
   GetToken(token); assert(strcmp(token, "numMaterials") == 0);
   num_materials = ReadInt();

   assert(num_materials < SIZE);

   size_t count = 0;
   while (num_materials > count)
   {
      GetToken(token); 

      if (strcmp(token, "Diffuse") == 0)
      {
         material[count] = ParseDiffuse();
      }
      else if (strcmp(token, "Reflective") == 0)
      {
         material[count] = ParseReflective();
      }
      else if (strcmp(token, "Glass") == 0)
      {
         material[count] = ParseGlass();
      }
      else if (strcmp(token, "Noise") == 0)
      {
         material[count] = ParseNoise(count);
      }
      else if (strcmp(token, "Marble") == 0)
      {
         material[count] = ParseMarble(count);
      }
      else if (strcmp(token, "Wood") == 0)
      {
         material[count] = ParseWood(count);
      }
      else if (strcmp(token, "Checkerboard") == 0)
      {
         material[count] = ParseCheckerboard(count);
      }

      ++count;
   }

   GetToken(token); assert(strcmp(token, "}") == 0);

   return;
}  

DiffuseMaterial* Scene::ParseDiffuse()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   color3f color(1.0f, 1.0f, 1.0f);
   color3f glow(0.0f, 0.0f, 0.0f);

   GetToken(token); assert(strcmp(token, "{") == 0);
   
   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "color") == 0)
      {
         color = ReadVector3f();
      }
      else if (strcmp(token, "glow") == 0)
      {
         glow = ReadVector3f();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   return new DiffuseMaterial(color, glow);
}

ReflectiveMaterial* Scene::ParseReflective()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   color3f color(1.0f, 1.0f, 1.0f);
   float blur = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "color") == 0)
      {
         color = ReadVector3f();
      }
      else if (strcmp(token, "blur") == 0)
      {
         blur = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   return new ReflectiveMaterial(color, blur);
}

GlassMaterial* Scene::ParseGlass()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   color3f color(0.92f, 0.92f, 0.92f);
   float index_of_refraction = 1.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "color") == 0)
      {
         color = ReadVector3f();
      }
      else if (strcmp(token, "indexOfRefraction") == 0)
      {
         index_of_refraction = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   return new GlassMaterial(color, index_of_refraction);
}

NoiseMaterial* Scene::ParseNoise(size_t count)
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   GetToken(token); assert(strcmp(token, "{") == 0);
   
   Matrix matrix;
          matrix.SetToIdentity();

   GetToken(token);
   
   if (strcmp(token, "Transform") == 0)
   {
      matrix.SetToIdentity();
      GetToken(token); assert(strcmp(token, "{") == 0);
      GetToken(token);

      for (;;)
      {
         if (strcmp(token, "Scale") == 0)
         {
            matrix = matrix * Matrix::MakeScale(ReadVector3f());
         }
         else if (strcmp(token, "UniformScale") == 0)
         {
            float s = ReadFloat();
            matrix = matrix * Matrix::MakeScale(vector3f(s, s, s));
         }
         else if (strcmp(token, "Translate") == 0)
         {
            matrix = matrix * Matrix::MakeTranslation(ReadVector3f());
         }
         else if (strcmp(token, "XRotate") == 0)
         {
            matrix = matrix * Matrix::MakeXRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "YRotate") == 0)
         {
            matrix = matrix * Matrix::MakeYRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "ZRotate") == 0)
         {
            matrix = matrix * Matrix::MakeZRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "Rotate") == 0)
         {
            GetToken(token); assert(strcmp(token, "{") == 0);
            vector3f axis = ReadVector3f();
            float degrees = ReadFloat();

            matrix = matrix * Matrix::MakeAxisRotation(axis, DegreesToRadians(degrees));
            
            GetToken(token); assert(strcmp(token, "}") == 0);
         }
         else if (strcmp(token, "Matrix") == 0)
         {
            Matrix matrix2;
                   matrix2.SetToIdentity();
         
            GetToken(token); assert(strcmp(token, "{") == 0);

            for (size_t j = 0; j < 4; ++j)
            {
               for (size_t i = 0; i < 4; ++i)
               {
                  float v = ReadFloat();
                  matrix2.Set(i, j, v);
               }
            }

            GetToken(token); assert(strcmp(token, "}") == 0);
            matrix = matrix2 * matrix;
         }
         else
         {
            break;
         }

         GetToken(token);
      }      
   }

   GetToken(token); assert(strcmp(token, "materialIndex") == 0);
   
   size_t m1 = ReadInt();
   assert(m1 < count);
   GetToken(token); assert(strcmp(token, "materialIndex") == 0);

   size_t m2 = ReadInt();
   assert(m2 < count);
   GetToken(token); assert(strcmp(token, "octaves") == 0);

   size_t octaves = ReadInt();
   GetToken(token); assert(strcmp(token, "}") == 0);

   return new NoiseMaterial(matrix, material[m1], material[m2], octaves);
}

MarbleMaterial* Scene::ParseMarble(size_t count)
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   GetToken(token); assert(strcmp(token, "{") == 0);

   Matrix matrix;
          matrix.SetToIdentity();

   GetToken(token); 

   if (strcmp(token, "Transform") == 0)
   {
      GetToken(token); assert(strcmp(token, "{") == 0);
      GetToken(token);

      for (;;)
      {
         if (strcmp(token, "Scale") == 0)
         {
            matrix = matrix * Matrix::MakeScale(ReadVector3f());
         }
         else if (strcmp(token, "UniformScale") == 0)
         {
            float s = ReadFloat();
            matrix = matrix * Matrix::MakeScale(vector3f(s, s, s));
         }
         else if (strcmp(token, "Translate") == 0)
         {
            matrix = matrix * Matrix::MakeTranslation(ReadVector3f());
         }
         else if (strcmp(token, "XRotate") == 0)
         {
            matrix = matrix * Matrix::MakeXRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "YRotate") == 0)
         {
            matrix = matrix * Matrix::MakeYRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "ZRotate") == 0)
         {
            matrix = matrix * Matrix::MakeZRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "Rotate") == 0)
         {
            GetToken(token); assert(strcmp(token, "{") == 0);
            vector3f axis = ReadVector3f();
            float degrees = ReadFloat();

            matrix = matrix * Matrix::MakeAxisRotation(axis, DegreesToRadians(degrees));
            
            GetToken(token); assert(strcmp(token, "}") == 0);
         }
         else if (strcmp(token, "Matrix") == 0)
         {
            Matrix matrix2;
                   matrix2.SetToIdentity();
         
            GetToken(token); assert(strcmp(token, "{") == 0);

            for (size_t j = 0; j < 4; ++j)
            {
               for (size_t i = 0; i < 4; ++i)
               {
                  float v = ReadFloat();
                  matrix2.Set(i, j, v);
               }
            }

            GetToken(token); assert(strcmp(token, "}") == 0);
            matrix = matrix2 * matrix;
         }
         else
         {
            break;
         }

         GetToken(token);
      }      
   }

   GetToken(token); assert(strcmp(token, "materialIndex") == 0);

   size_t m1 = ReadInt();
   assert(m1 < count);
   GetToken(token); assert(strcmp(token, "materialIndex") == 0);

   size_t m2 = ReadInt();
   assert(m2 < count);
   GetToken(token); assert(strcmp(token, "octaves") == 0);

   size_t octaves = ReadInt();
   GetToken(token); assert(strcmp(token, "frequency") == 0);

   float frequency = ReadFloat();
   GetToken(token); assert(strcmp(token, "amplitude") == 0);

   float amplitude = ReadFloat();
   GetToken(token); assert(strcmp(token, "}") == 0);

   return new MarbleMaterial(matrix, material[m1], material[m2], octaves, frequency, amplitude);
}

WoodMaterial* Scene::ParseWood(size_t count)
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   GetToken(token); assert(strcmp(token, "{") == 0);

   Matrix matrix;
          matrix.SetToIdentity();

   GetToken(token); 

   if (strcmp(token, "Transform") == 0)
   {
      GetToken(token); assert(strcmp(token, "{") == 0);
      GetToken(token);

      for (;;)
      {
         if (strcmp(token, "Scale") == 0)
         {
            matrix = matrix * Matrix::MakeScale(ReadVector3f());
         }
         else if (strcmp(token, "UniformScale") == 0)
         {
            float s = ReadFloat();
            matrix = matrix * Matrix::MakeScale(vector3f(s, s, s));
         }
         else if (strcmp(token, "Translate") == 0)
         {
            matrix = matrix * Matrix::MakeTranslation(ReadVector3f());
         }
         else if (strcmp(token, "XRotate") == 0)
         {
            matrix = matrix * Matrix::MakeXRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "YRotate") == 0)
         {
            matrix = matrix * Matrix::MakeYRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "ZRotate") == 0)
         {
            matrix = matrix * Matrix::MakeZRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "Rotate") == 0)
         {
            GetToken(token); assert(strcmp(token, "{") == 0);
            vector3f axis = ReadVector3f();
            float degrees = ReadFloat();

            matrix = matrix * Matrix::MakeAxisRotation(axis, DegreesToRadians(degrees));
            
            GetToken(token); assert(strcmp(token, "}") == 0);
         }
         else if (strcmp(token, "Matrix") == 0)
         {
            Matrix matrix2;
                   matrix2.SetToIdentity();
         
            GetToken(token); assert(strcmp(token, "{") == 0);

            for (size_t j = 0; j < 4; ++j)
            {
               for (size_t i = 0; i < 4; ++i)
               {
                  float v = ReadFloat();
                  matrix2.Set(i, j, v);
               }
            }

            GetToken(token); assert(strcmp(token, "}") == 0);
            matrix = matrix2 * matrix;
         }
         else
         {
            break;
         }

         GetToken(token);
      }      
   }

   GetToken(token); assert(strcmp(token, "materialIndex") == 0);

   size_t m1 = ReadInt();
   assert(m1 < count);
   GetToken(token); assert(strcmp(token, "materialIndex") == 0);

   size_t m2 = ReadInt();
   assert(m2 < count);
   GetToken(token); assert(strcmp(token, "octaves") == 0);

   size_t octaves = ReadInt();
   GetToken(token); assert(strcmp(token, "frequency") == 0);

   float frequency = ReadFloat();
   GetToken(token); assert(strcmp(token, "amplitude") == 0);

   float amplitude = ReadFloat();
   GetToken(token); assert(strcmp(token, "}") == 0);

   return new WoodMaterial(matrix, material[m1], material[m2], octaves, frequency, amplitude);
}

Checkerboard* Scene::ParseCheckerboard(size_t count)
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   GetToken(token); assert(strcmp(token, "{") == 0);
   Matrix matrix;
          matrix.SetToIdentity();

   GetToken(token); 
   
   if (strcmp(token, "Transform") == 0)
   {
      GetToken(token); assert(strcmp(token, "{") == 0);
      GetToken(token);

      for (;;)
      {
         if (strcmp(token, "Scale") == 0)
         {
            matrix = matrix * Matrix::MakeScale(ReadVector3f());
         }
         else if (strcmp(token, "UniformScale") == 0)
         {
            float s = ReadFloat();
            matrix = matrix * Matrix::MakeScale(vector3f(s, s, s));
         }
         else if (strcmp(token, "Translate") == 0)
         {
            matrix = matrix * Matrix::MakeTranslation(ReadVector3f());
         }
         else if (strcmp(token, "XRotate") == 0)
         {
            matrix = matrix * Matrix::MakeXRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "YRotate") == 0)
         {
            matrix = matrix * Matrix::MakeYRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "ZRotate") == 0)
         {
            matrix = matrix * Matrix::MakeZRotation(DegreesToRadians(ReadFloat()));
         }
         else if (strcmp(token, "Rotate") == 0)
         {
            GetToken(token); assert(strcmp(token, "{") == 0);
            vector3f axis = ReadVector3f();
            float degrees = ReadFloat();

            matrix = matrix * Matrix::MakeAxisRotation(axis, DegreesToRadians(degrees));
            
            GetToken(token); assert(strcmp(token, "}") == 0);
         }
         else if (strcmp(token, "Matrix") == 0)
         {
            Matrix matrix2;
                   matrix2.SetToIdentity();
         
            GetToken(token); assert(strcmp(token, "{") == 0);

            for (size_t j = 0; j < 4; ++j)
            {
               for (size_t i = 0; i < 4; ++i)
               {
                  float v = ReadFloat();
                  matrix2.Set(i, j, v);
               }
            }

            GetToken(token); assert(strcmp(token, "}") == 0);
            matrix = matrix2 * matrix;
         }
         else
         {
            break;
         }

         GetToken(token);
      }      
   }

   GetToken(token); assert(strcmp(token, "materialIndex") == 0);

   size_t m1 = ReadInt();
   assert(m1 >= 0 && m1 < count);
   
   GetToken(token); assert(strcmp(token, "materialIndex") == 0);
   
   size_t m2 = ReadInt();
   assert(m2 >= 0 && m2 < count);
   
   GetToken(token); assert(strcmp(token, "}") == 0);

   return new Checkerboard(matrix, material[m1], material[m2]);
}

Object* Scene::ParseObject(char token[MAX_PARSER_TOKEN_LENGTH])
{
   Object* object = NULL;

   if (strcmp(token, "Group") == 0)
   {
      object = ParseGroup();
   }
   else if (strcmp(token, "CSGPair") == 0)
   {
      object = ParseCSGPair();
   }
   else if (strcmp(token, "Sphere") == 0)
   {
      object = ParseSphere();
   }
   else if (strcmp(token, "MotionSphere") == 0)
   {
      object = ParseMotionSphere();
      distribution = true;
   }
   else if (strcmp(token, "Plane") == 0)
   {
      object = ParsePlane();
   }
   else if (strcmp(token, "Triangle") == 0)
   {
      object = ParseTriangle();
   }
   else if (strcmp(token, "Cone") == 0)
   {
      object = ParseCone();
   }
   else if (strcmp(token, "XYRectangle") == 0)
   {
      object = ParseXYRectangle();
   }
   else if (strcmp(token, "XZRectangle") == 0)
   {
      object = ParseXZRectangle();
   }
   else if (strcmp(token, "YZRectangle") == 0)
   {
      object = ParseYZRectangle();
   }
   else if (strcmp(token, "TriangleMesh") == 0)
   {
      object = ParseTriangleMesh();
   }
   else if (strcmp(token, "Cube") == 0)
   {
      object = ParseCube();
   }
   else if (strcmp(token, "Transform") == 0)
   {
      object = ParseTransform();
   }

   return object;
}

Group* Scene::ParseGroup()
{
/* Each group starts with an integer that specifies the number of objects in 
   the group.
  
   The material index sets the material of all objects which follow,
   until the next material index (scoping for the materials is very
   simple, and essentially ignores any tree hierarchy). */

   char token[MAX_PARSER_TOKEN_LENGTH];

   GetToken(token); assert(strcmp(token, "{") == 0);

   GetToken(token); assert(strcmp(token, "numObjects") == 0);
   size_t num_objects = ReadInt();

   Group* result = new Group(num_objects);

   size_t count = 0;
   while (num_objects > count)
   {
      GetToken(token);
   
      if (strcmp(token, "MaterialIndex") == 0)
      {
         size_t index = ReadInt();
         assert(index >= 0 && index <= GetNumMaterials());
         current_material = GetMaterial(index);
      }
      else
      {
         Object* object = ParseObject(token);
         assert(object != NULL);
         result->SetAt(count, object);
         count++;
      }
   }
   
   GetToken(token); assert(strcmp(token, "}") == 0);

   return result;
}

CSGPair* Scene::ParseCSGPair()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   GetToken(token); assert(strcmp(token, "{") == 0);

   CSGPair::Type t = CSGPair::Type::Union;
   Solid* a = NULL, * b = NULL;

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "type") == 0)
      {
         GetToken(token);

         if (strcmp(token, "Intersection") == 0)
         {
            t = CSGPair::Type::Intersection;
         }
         else if (strcmp(token, "Difference") == 0)
         {
            t = CSGPair::Type::Difference;
         }
      }
      else if (strcmp(token, "MaterialIndex") == 0)
      {
         size_t index = ReadInt();
         assert(index <= GetNumMaterials());
         current_material = GetMaterial(index);
      }
      else if (strcmp(token, "Cube") == 0)
      {
         if (a == NULL)
         {
            a = ParseCube();
            assert(a != NULL);
         }
         else if (b == NULL)
         {
            b = ParseCube();
            assert(b != NULL);
         }
      }
      else if (strcmp(token, "Sphere") == 0)
      {
         if (a == NULL)
         {
            a = ParseSphere();
            assert(a != NULL);
         }
         else if (b == NULL)
         {
            b = ParseSphere();
            assert(b != NULL);
         }
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }

   }

   CSGPair* result = new CSGPair(a, b);
   result->SetType(t);

   return result;
}

Sphere* Scene::ParseSphere()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point3f center;
   float radius = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "center") == 0)
      {
         center = ReadVector3f();
      }
      else if (strcmp(token, "radius") == 0)
      {
         radius = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);
   
   return new Sphere(center, radius, current_material);
}

MotionSphere* Scene::ParseMotionSphere()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point3f center;
   vector3f velocity;
   float radius = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "center") == 0)
      {
         center = ReadVector3f();
      }
      else if (strcmp(token, "radius") == 0)
      {
         radius = ReadFloat();
      }
      else if (strcmp(token, "velocity") == 0)
      {
         velocity = ReadVector3f();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);
   
   return new MotionSphere(center, radius, velocity, current_material);
}

Plane* Scene::ParsePlane()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   vector3f normal;
   float offset = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "normal") == 0)
      {
         normal = ReadVector3f();
      }
      else if (strcmp(token, "offset") == 0)
      {
         offset = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);

   return new Plane(normal, offset, current_material);
}

Triangle* Scene::ParseTriangle()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point3f v0, v1, v2;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "vertex0") == 0)
      {
         v0 = ReadVector3f();
      }
      else if (strcmp(token, "vertex1") == 0)
      {
         v1 = ReadVector3f();
      }
      else if (strcmp(token, "vertex2") == 0)
      {
         v2 = ReadVector3f();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);

   return new Triangle(v0, v1, v2, current_material);
}

Cone* Scene::ParseCone()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point3f v;
   vector3f axis;
   float a = 0.0f, h = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "tip") == 0)
      {
         v = ReadVector3f();
      }
      else if (strcmp(token, "axis") == 0)
      {
         axis = ReadVector3f();
      }
      else if (strcmp(token, "angle") == 0)
      {
         a = ReadFloat();

         a = (float) cos(DegreesToRadians(a));
      }
      else if (strcmp(token, "height") == 0)
      {
         h = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);

   return new Cone(v, axis, a, h, current_material);
}

XYRectangle* Scene::ParseXYRectangle()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point2f v0, v1;
   float k = 0.0f, n = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "lower") == 0)
      {
         v0 = ReadVector2f();
      }
      else if (strcmp(token, "upper") == 0)
      {
         v1 = ReadVector2f();
      }
      else if (strcmp(token, "normal") == 0)
      {
         n = ReadFloat();
      }
      else if (strcmp(token, "k") == 0)
      {
         k = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);

   return new XYRectangle(v0, v1, k, n, current_material);
}

XZRectangle* Scene::ParseXZRectangle()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point2f v0, v1;
   float k = 0.0f, n = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "lower") == 0)
      {
         v0 = ReadVector2f();
      }
      else if (strcmp(token, "upper") == 0)
      {
         v1 = ReadVector2f();
      }
      else if (strcmp(token, "normal") == 0)
      {
         n = ReadFloat();
      }
      else if (strcmp(token, "k") == 0)
      {
         k = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);

   return new XZRectangle(v0, v1, k, n, current_material);
}

YZRectangle* Scene::ParseYZRectangle()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point2f v0, v1;
   float k = 0.0f, n = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "lower") == 0)
      {
         v0 = ReadVector2f();
      }
      else if (strcmp(token, "upper") == 0)
      {
         v1 = ReadVector2f();
      }
      else if (strcmp(token, "normal") == 0)
      {
         n = ReadFloat();
      }
      else if (strcmp(token, "k") == 0)
      {
         k = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);

   return new YZRectangle(v0, v1, k, n, current_material);
}

Group* Scene::ParseTriangleMesh()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   char szFileName[MAX_PARSER_TOKEN_LENGTH];

   assert(current_material != NULL);

   GetToken(token); assert(strcmp(token, "{") == 0);
   GetToken(token); assert(strcmp(token, "file") == 0);
   GetToken(szFileName); 
   GetToken(token); assert(strcmp(token, "}") == 0);

   FILE* f = fopen(szFileName, "r");
   assert(f != NULL);

   int vcount = 0, fcount = 0;
   
   int c = fgetc(f);

   while (c != EOF)
   {
      if (c == 'v')
      { 
         assert(fcount == 0);
         float v0 = 0.0f, v1 = 0.0f, v2 = 0.0f;
         fscanf(f, "%f %f %f", &v0, &v1, &v2);
         ++vcount; 
      }
      else if (c == 'f')
      {
         int f0 = 0, f1 = 0, f2 = 0;
         fscanf(f, "%d %d %d", &f0, &f1, &f2);
         ++fcount; 
      }
      else
      {
         assert(c == '\n');
      }

      c = fgetc(f);
   }

   fseek(f, 0, SEEK_SET);

   vector3f* verts = new vector3f[vcount];
   Group* result   = new Group(fcount);

   int new_vcount = 0, new_fcount = 0;
  
   vector3f vmax(-FLT_MAX, -FLT_MAX, -FLT_MAX), vmin(FLT_MAX, FLT_MAX, FLT_MAX);

   c = fgetc(f);

   while (c != EOF)
   {    
      if (c == 'v')
      { 
         assert(new_fcount == 0);
         float v0 = 0.0f, v1 = 0.0f, v2 = 0.0f;
         fscanf(f, "%f %f %f", &v0, &v1, &v2);
         verts[new_vcount] = vector3f(v0, v1, v2);

         if (vmin[x] > v0) vmin[x] = v0;
         if (vmin[y] > v1) vmin[y] = v1;
         if (vmin[z] > v2) vmin[z] = v2;

         if (vmax[x] < v0) vmax[x] = v0;
         if (vmax[y] < v1) vmax[y] = v1;
         if (vmax[z] < v2) vmax[z] = v2;

         ++new_vcount;
      }
      else if (c == 'f')
      {
         assert(vcount == new_vcount);
         int f0 = 0, f1 = 0, f2 = 0;
         fscanf(f, "%d %d %d", &f0, &f1, &f2);
         assert(f0 > 0 && f0 <= vcount);
         assert(f1 > 0 && f1 <= vcount);
         assert(f2 > 0 && f2 <= vcount);
         Triangle* t = new Triangle(verts[f0 - 1], verts[f1 - 1], verts[f2 - 1], current_material);
         result->SetAt(new_fcount, t);
         ++new_fcount;
      }

      c = fgetc(f);
   }

   fclose(f);

   delete [] verts;
   assert(fcount == new_fcount);
   assert(vcount == new_vcount);

   result->SetBB(vmin * 1.01f, vmax * 1.01f); /* Increase the Bounding Box by a little (1%). */

   return result;
}

Cube* Scene::ParseCube()
{
   char token[MAX_PARSER_TOKEN_LENGTH];
   point3f center;
   float size = 0.0f;

   GetToken(token); assert(strcmp(token, "{") == 0);

   for (;;)
   {
      GetToken(token);

      if (strcmp(token, "center") == 0)
      {
         center = ReadVector3f();
      }
      else if (strcmp(token, "size") == 0)
      {
         size = ReadFloat();
      }
      else
      {
         assert(strcmp(token, "}") == 0);
         break;
      }
   }

   assert(current_material != NULL);

   return new Cube(center, size, current_material);
}

Transform* Scene::ParseTransform()
{
   char token[MAX_PARSER_TOKEN_LENGTH];

   Matrix matrix;
          matrix.SetToIdentity();
   Object* object = NULL;
  
   GetToken(token); assert(strcmp(token, "{") == 0);
   GetToken(token);
  
   for (;;)
   {
      if (!strcmp(token, "Scale"))
      {
         matrix = matrix * Matrix::MakeScale(ReadVector3f());
      }
      else if (strcmp(token, "UniformScale") == 0)
      {
         float s = ReadFloat();
         matrix = matrix * Matrix::MakeScale(vector3f(s, s, s));
      }
      else if (strcmp(token, "Translate") == 0)
      {
         matrix = matrix * Matrix::MakeTranslation(ReadVector3f());
      }
      else if (strcmp(token, "XRotate") == 0)
      {
         matrix = matrix * Matrix::MakeXRotation(DegreesToRadians(ReadFloat()));
      }
      else if (strcmp(token, "YRotate") == 0)
      {
         matrix = matrix * Matrix::MakeYRotation(DegreesToRadians(ReadFloat()));
      }
      else if (strcmp(token, "ZRotate") == 0)
      {
         matrix = matrix * Matrix::MakeZRotation(DegreesToRadians(ReadFloat()));
      }
      else if (strcmp(token, "Rotate") == 0)
      {
         GetToken(token); assert(strcmp(token, "{") == 0);
         vector3f axis = ReadVector3f();
         float degrees = ReadFloat();

         matrix = matrix * Matrix::MakeAxisRotation(axis, DegreesToRadians(degrees));
         
         GetToken(token); assert(strcmp(token, "}") == 0);
      }
      else if (strcmp(token, "Matrix") == 0)
      {
         Matrix matrix2;
                matrix2.SetToIdentity();
      
         GetToken(token); assert(strcmp(token, "{") == 0);

         for (size_t j = 0; j < 4; ++j)
         {
            for (size_t i = 0; i < 4; ++i)
            {
               float v = ReadFloat();
               matrix2.Set(i, j, v);
            }
         }

         GetToken(token); assert(strcmp(token, "}") == 0);
         matrix = matrix2 * matrix;
      }
      else
      {
      /* Otherwise this must be an object, and there are no more transformations. */
         object = ParseObject(token);
         break;
      }

      GetToken(token);
   }

   assert(object != NULL);
   GetToken(token); assert(strcmp(token, "}") == 0);

   return new Transform(matrix, object);
}

bool Scene::GetToken(char token[MAX_PARSER_TOKEN_LENGTH])
{
   token[0] = '\0';
   int success = fscanf(file, "%s ", token);

   while (success != EOF)
   {
      if (token[0] == '#')
      {
         while (getc(file) != '\n');

         token[0] = '\0';
         success = fscanf(file, "%s ", token);
      }
      else
      {
         break;
      }
   }

   return success != EOF;
}

vector3f Scene::ReadVector3f()
{
   float t = 0.0f, u = 0.0f, v = 0.0f;

   fscanf(file, "%f %f %f",&t, &u, &v);

   return vector3f(t, u, v);
}

vector2f Scene::ReadVector2f()
{
   float u = 0.0f, v = 0.0f;
   
   fscanf(file, "%f %f", &u, &v);

   return vector2f(u, v);
}

float Scene::ReadFloat()
{
   float r = 0.0f;

   fscanf(file, "%f", &r);

   return r;
}

int Scene::ReadInt()
{
   int r = 0;

   fscanf(file, "%d", &r);
   
   return r;
}
