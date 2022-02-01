/* File: hit.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;              */

#ifndef HIT_H
#define HIT_H

#include <float.h>

#include "math.h"
#include "ray.h"

class Material;

class Hit
{
public:
   Hit(float t = FLT_MAX) : tmin(t),  material(NULL) { }

   float GetT()            const {   return tmin;   }
   Material* GetMaterial() const {   return material;   }
   point3f GetIntersectionPoint() const {   return intersection_point;   }
   vector3f GetNormal()    const {   return normal;   }

   void Set(float t, Material* m, const vector3f& n, const Ray& ray)
   {
      tmin = t;
      material = m;
      normal = n;

      intersection_point = ray.PointAtParameter(tmin);

      return;
   }

protected:
private: 
   float tmin;
   Material* material;
   vector3f normal;
   point3f intersection_point;
};

#endif
