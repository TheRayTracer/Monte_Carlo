/* File: ray.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;              */

#ifndef RAY_H
#define RAY_H

#include "math.h"

/* Ray class mostly copied from Peter Shirley and Keith Morley. */

class Ray
{
public:
   Ray(const point3f& o, const vector3f& d) : origin(o), direction(d)
   {
      direction.Normalize();

      inverse_direction = vector3f(1.0f / direction[x], 1.0f / direction[y], 1.0f / direction[z]);
   }

   point3f GetOrigin()    const {   return origin;      }
   vector3f GetDirection() const {   return direction;   }
  
   vector3f GetInverseDirectionForAABoxFaceIntersection() const { return inverse_direction; }

   point3f PointAtParameter(float t) const
   {
      return origin + direction * t;
   }

protected:
private:
   point3f origin;
   vector3f direction;

   vector3f inverse_direction;
};

#endif
