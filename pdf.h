/* File: pdf.h; Mode: C++; Tab-width: 3; Author: Simon Flannery;              */

#ifndef PDF_H
#define PDF_H

#include "math.h"

class Pdf
{
public:
   virtual float GetValue(const vector3f& direction) const = 0;
   virtual vector3f Generate() const = 0;

protected:
   static vector3f RandomCosineDirection() 
   {
      float r1 = random_float();
      float r2 = random_float();

      float _z = (float) sqrt(1.0f - r2);

      float phi = 2.0f * PI * r1;
      float _x = (float) cos(phi) * (float) sqrt(r2);
      float _y = (float) sin(phi) * (float) sqrt(r2);

      return vector3f(_x, _y, _z);
   }
};

class CosinePdf : public Pdf
{
public:
   CosinePdf(const vector3f& n)
   {
      w = n;
      w.Normalize();

      vector3f a = (fabs(w[x]) > 0.9f) ? vector3f(0.0f, 1.0f, 0.0f) : vector3f(1.0f, 0.0f, 0.0f);

      v = vector3f::Cross(w, a).Normalize();
      u = vector3f::Cross(w, v).Normalize();
   }

   virtual float GetValue(const vector3f& direction) const
   {
      float cosine = vector3f::Dot(w, direction);

      return (float) fmax(FLT_EPSILON, cosine / PI);
   }

   virtual vector3f Generate() const
   {
      vector3f d = RandomCosineDirection();

      return d[x] * u + d[y] * v + d[z] * w;
   }

public:
   vector3f u, v, w;
};

#endif
